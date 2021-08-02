/** @file
*  File managing the MMU for ARMv8 architecture
*
*  Copyright (c) 2011-2014, ARM Limited. All rights reserved.
*  Copyright (c) 2016, Linaro Limited. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

#include <Uefi.h>
#include <Chipset/AArch64.h>
#include <Library/BaseMemoryLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ArmLib.h>
#include <Library/ArmMmuLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>

// We use this index definition to define an invalid block entry
#define TT_ATTR_INDX_INVALID    ((UINT32)~0)

STATIC
UINT64
ArmMemoryAttributeToPageAttribute (
  IN ARM_MEMORY_REGION_ATTRIBUTES  Attributes
  )
{
  switch (Attributes) {
  case ARM_MEMORY_REGION_ATTRIBUTE_WRITE_BACK:
  case ARM_MEMORY_REGION_ATTRIBUTE_NONSECURE_WRITE_BACK:
    return TT_ATTR_INDX_MEMORY_WRITE_BACK | TT_SH_INNER_SHAREABLE;

  case ARM_MEMORY_REGION_ATTRIBUTE_WRITE_THROUGH:
  case ARM_MEMORY_REGION_ATTRIBUTE_NONSECURE_WRITE_THROUGH:
    return TT_ATTR_INDX_MEMORY_WRITE_THROUGH | TT_SH_INNER_SHAREABLE;

  // Uncached and device mappings are treated as outer shareable by default,
  case ARM_MEMORY_REGION_ATTRIBUTE_UNCACHED_UNBUFFERED:
  case ARM_MEMORY_REGION_ATTRIBUTE_NONSECURE_UNCACHED_UNBUFFERED:
    return TT_ATTR_INDX_MEMORY_NON_CACHEABLE;

  default:
    ASSERT(0);
  case ARM_MEMORY_REGION_ATTRIBUTE_DEVICE:
  case ARM_MEMORY_REGION_ATTRIBUTE_NONSECURE_DEVICE:
    if (ArmReadCurrentEL () == AARCH64_EL2)
      return TT_ATTR_INDX_DEVICE_MEMORY | TT_XN_MASK;
    else
      return TT_ATTR_INDX_DEVICE_MEMORY | TT_UXN_MASK | TT_PXN_MASK;
  }
}

UINT64
PageAttributeToGcdAttribute (
  IN UINT64 PageAttributes
  )
{
  UINT64  GcdAttributes;

  switch (PageAttributes & TT_ATTR_INDX_MASK) {
  case TT_ATTR_INDX_DEVICE_MEMORY:
    GcdAttributes = EFI_MEMORY_UC;
    break;
  case TT_ATTR_INDX_MEMORY_NON_CACHEABLE:
    GcdAttributes = EFI_MEMORY_WC;
    break;
  case TT_ATTR_INDX_MEMORY_WRITE_THROUGH:
    GcdAttributes = EFI_MEMORY_WT;
    break;
  case TT_ATTR_INDX_MEMORY_WRITE_BACK:
    GcdAttributes = EFI_MEMORY_WB;
    break;
  default:
    DEBUG ((EFI_D_ERROR, "PageAttributeToGcdAttribute: PageAttributes:0x%lX not supported.\n", PageAttributes));
    ASSERT (0);
    // The Global Coherency Domain (GCD) value is defined as a bit set.
    // Returning 0 means no attribute has been set.
    GcdAttributes = 0;
  }

  // Determine protection attributes
  if (((PageAttributes & TT_AP_MASK) == TT_AP_NO_RO) || ((PageAttributes & TT_AP_MASK) == TT_AP_RO_RO)) {
    // Read only cases map to write-protect
    GcdAttributes |= EFI_MEMORY_WP;
  }

  // Process eXecute Never attribute
  if ((PageAttributes & (TT_PXN_MASK | TT_UXN_MASK)) != 0 ) {
    GcdAttributes |= EFI_MEMORY_XP;
  }

  return GcdAttributes;
}

ARM_MEMORY_REGION_ATTRIBUTES
GcdAttributeToArmAttribute (
  IN UINT64 GcdAttributes
  )
{
  switch (GcdAttributes & 0xFF) {
  case EFI_MEMORY_UC:
    return ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;
  case EFI_MEMORY_WC:
    return ARM_MEMORY_REGION_ATTRIBUTE_UNCACHED_UNBUFFERED;
  case EFI_MEMORY_WT:
    return ARM_MEMORY_REGION_ATTRIBUTE_WRITE_THROUGH;
  case EFI_MEMORY_WB:
    return ARM_MEMORY_REGION_ATTRIBUTE_WRITE_BACK;
  default:
    DEBUG ((EFI_D_ERROR, "GcdAttributeToArmAttribute: 0x%lX attributes is not supported.\n", GcdAttributes));
    ASSERT (0);
    return ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;
  }
}

#define MIN_T0SZ        16
#define BITS_PER_LEVEL  9

VOID
GetRootTranslationTableInfo (
  IN UINTN     T0SZ,
  OUT UINTN   *TableLevel,
  OUT UINTN   *TableEntryCount
  )
{
  // Get the level of the root table
  if (TableLevel) {
    *TableLevel = (T0SZ - MIN_T0SZ) / BITS_PER_LEVEL;
  }

  if (TableEntryCount) {
    *TableEntryCount = 1UL << (BITS_PER_LEVEL - (T0SZ - MIN_T0SZ) % BITS_PER_LEVEL);
  }
}

STATIC
VOID
ReplaceLiveEntry (
  IN  UINT64  *Entry,
  IN  UINT64  Value
  )
{
  if (!ArmMmuEnabled ()) {
    *Entry = Value;
  } else {
    ArmReplaceLiveTranslationEntry (Entry, Value);
  }
}

STATIC
VOID
LookupAddresstoRootTable (
  IN  UINT64  MaxAddress,
  OUT UINTN  *T0SZ,
  OUT UINTN  *TableEntryCount
  )
{
  UINTN TopBit;

  // Check the parameters are not NULL
  ASSERT ((T0SZ != NULL) && (TableEntryCount != NULL));

  // Look for the highest bit set in MaxAddress
  for (TopBit = 63; TopBit != 0; TopBit--) {
    if ((1ULL << TopBit) & MaxAddress) {
      // MaxAddress top bit is found
      TopBit = TopBit + 1;
      break;
    }
  }
  ASSERT (TopBit != 0);

  // Calculate T0SZ from the top bit of the MaxAddress
  *T0SZ = 64 - TopBit;

  // Get the Table info from T0SZ
  GetRootTranslationTableInfo (*T0SZ, NULL, TableEntryCount);
}

STATIC
UINT64*
GetBlockEntryListFromAddress (
  IN  UINT64       *RootTable,
  IN  UINT64        RegionStart,
  OUT UINTN        *TableLevel,
  IN OUT UINT64    *BlockEntrySize,
  OUT UINT64      **LastBlockEntry
  )
{
  UINTN   RootTableLevel;
  UINTN   RootTableEntryCount;
  UINT64 *TranslationTable;
  UINT64 *BlockEntry;
  UINT64 *SubTableBlockEntry;
  UINT64  BlockEntryAddress;
  UINTN   BaseAddressAlignment;
  UINTN   PageLevel;
  UINTN   Index;
  UINTN   IndexLevel;
  UINTN   T0SZ;
  UINT64  Attributes;
  UINT64  TableAttributes;

  // Initialize variable
  BlockEntry = NULL;

  // Ensure the parameters are valid
  if (!(TableLevel && BlockEntrySize && LastBlockEntry)) {
    ASSERT_EFI_ERROR (EFI_INVALID_PARAMETER);
    return NULL;
  }

  // Ensure the Region is aligned on 4KB boundary
  if ((RegionStart & (SIZE_4KB - 1)) != 0) {
    ASSERT_EFI_ERROR (EFI_INVALID_PARAMETER);
    return NULL;
  }

  // Ensure the required size is aligned on 4KB boundary and not 0
  if ((*BlockEntrySize & (SIZE_4KB - 1)) != 0 || *BlockEntrySize == 0) {
    ASSERT_EFI_ERROR (EFI_INVALID_PARAMETER);
    return NULL;
  }

  T0SZ = ArmGetTCR () & TCR_T0SZ_MASK;
  // Get the Table info from T0SZ
  GetRootTranslationTableInfo (T0SZ, &RootTableLevel, &RootTableEntryCount);

  // If the start address is 0x0 then we use the size of the region to identify the alignment
  if (RegionStart == 0) {
    // Identify the highest possible alignment for the Region Size
    BaseAddressAlignment = LowBitSet64 (*BlockEntrySize);
  } else {
    // Identify the highest possible alignment for the Base Address
    BaseAddressAlignment = LowBitSet64 (RegionStart);
  }

  // Identify the Page Level the RegionStart must belong to. Note that PageLevel
  // should be at least 1 since block translations are not supported at level 0
  PageLevel = MAX (3 - ((BaseAddressAlignment - 12) / 9), 1);

  // If the required size is smaller than the current block size then we need to go to the page below.
  // The PageLevel was calculated on the Base Address alignment but did not take in account the alignment
  // of the allocation size
  while (*BlockEntrySize < TT_BLOCK_ENTRY_SIZE_AT_LEVEL (PageLevel)) {
    // It does not fit so we need to go a page level above
    PageLevel++;
  }

  //
  // Get the Table Descriptor for the corresponding PageLevel. We need to decompose RegionStart to get appropriate entries
  //

  TranslationTable = RootTable;
  for (IndexLevel = RootTableLevel; IndexLevel <= PageLevel; IndexLevel++) {
    BlockEntry = (UINT64*)TT_GET_ENTRY_FOR_ADDRESS (TranslationTable, IndexLevel, RegionStart);

    if ((IndexLevel != 3) && ((*BlockEntry & TT_TYPE_MASK) == TT_TYPE_TABLE_ENTRY)) {
      // Go to the next table
      TranslationTable = (UINT64*)(*BlockEntry & TT_ADDRESS_MASK_DESCRIPTION_TABLE);

      // If we are at the last level then update the last level to next level
      if (IndexLevel == PageLevel) {
        // Enter the next level
        PageLevel++;
      }
    } else if ((*BlockEntry & TT_TYPE_MASK) == TT_TYPE_BLOCK_ENTRY) {
      // If we are not at the last level then we need to split this BlockEntry
      if (IndexLevel != PageLevel) {
        // Retrieve the attributes from the block entry
        Attributes = *BlockEntry & TT_ATTRIBUTES_MASK;

        // Convert the block entry attributes into Table descriptor attributes
        TableAttributes = TT_TABLE_AP_NO_PERMISSION;
        if (Attributes & TT_NS) {
          TableAttributes = TT_TABLE_NS;
        }

        // Get the address corresponding at this entry
        BlockEntryAddress = RegionStart;
        BlockEntryAddress = BlockEntryAddress >> TT_ADDRESS_OFFSET_AT_LEVEL(IndexLevel);
        // Shift back to right to set zero before the effective address
        BlockEntryAddress = BlockEntryAddress << TT_ADDRESS_OFFSET_AT_LEVEL(IndexLevel);

        // Set the correct entry type for the next page level
        if ((IndexLevel + 1) == 3) {
          Attributes |= TT_TYPE_BLOCK_ENTRY_LEVEL3;
        } else {
          Attributes |= TT_TYPE_BLOCK_ENTRY;
        }

        // Create a new translation table
        TranslationTable = AllocatePages (1);
        if (TranslationTable == NULL) {
          return NULL;
        }

        // Populate the newly created lower level table
        SubTableBlockEntry = TranslationTable;
        for (Index = 0; Index < TT_ENTRY_COUNT; Index++) {
          *SubTableBlockEntry = Attributes | (BlockEntryAddress + (Index << TT_ADDRESS_OFFSET_AT_LEVEL(IndexLevel + 1)));
          SubTableBlockEntry++;
        }

        // Fill the BlockEntry with the new TranslationTable
        ReplaceLiveEntry (BlockEntry,
          ((UINTN)TranslationTable & TT_ADDRESS_MASK_DESCRIPTION_TABLE) | TableAttributes | TT_TYPE_TABLE_ENTRY);
      }
    } else {
      if (IndexLevel != PageLevel) {
        //
        // Case when we have an Invalid Entry and we are at a page level above of the one targetted.
        //

        // Create a new translation table
        TranslationTable = AllocatePages (1);
        if (TranslationTable == NULL) {
          return NULL;
        }

        ZeroMem (TranslationTable, TT_ENTRY_COUNT * sizeof(UINT64));

        // Fill the new BlockEntry with the TranslationTable
        *BlockEntry = ((UINTN)TranslationTable & TT_ADDRESS_MASK_DESCRIPTION_TABLE) | TT_TYPE_TABLE_ENTRY;
      }
    }
  }

  // Expose the found PageLevel to the caller
  *TableLevel = PageLevel;

  // Now, we have the Table Level we can get the Block Size associated to this table
  *BlockEntrySize = TT_BLOCK_ENTRY_SIZE_AT_LEVEL (PageLevel);

  // The last block of the root table depends on the number of entry in this table,
  // otherwise it is always the (TT_ENTRY_COUNT - 1)th entry in the table.
  *LastBlockEntry = TT_LAST_BLOCK_ADDRESS(TranslationTable,
      (PageLevel == RootTableLevel) ? RootTableEntryCount : TT_ENTRY_COUNT);

  return BlockEntry;
}

STATIC
RETURN_STATUS
UpdateRegionMapping (
  IN  UINT64  *RootTable,
  IN  UINT64  RegionStart,
  IN  UINT64  RegionLength,
  IN  UINT64  Attributes,
  IN  UINT64  BlockEntryMask
  )
{
  UINT32  Type;
  UINT64  *BlockEntry;
  UINT64  *LastBlockEntry;
  UINT64  BlockEntrySize;
  UINTN   TableLevel;

  // Ensure the Length is aligned on 4KB boundary
  if ((RegionLength == 0) || ((RegionLength & (SIZE_4KB - 1)) != 0)) {
    ASSERT_EFI_ERROR (EFI_INVALID_PARAMETER);
    return RETURN_INVALID_PARAMETER;
  }

  do {
    // Get the first Block Entry that matches the Virtual Address and also the information on the Table Descriptor
    // such as the the size of the Block Entry and the address of the last BlockEntry of the Table Descriptor
    BlockEntrySize = RegionLength;
    BlockEntry = GetBlockEntryListFromAddress (RootTable, RegionStart, &TableLevel, &BlockEntrySize, &LastBlockEntry);
    if (BlockEntry == NULL) {
      // GetBlockEntryListFromAddress() return NULL when it fails to allocate new pages from the Translation Tables
      return RETURN_OUT_OF_RESOURCES;
    }

    if (TableLevel != 3) {
      Type = TT_TYPE_BLOCK_ENTRY;
    } else {
      Type = TT_TYPE_BLOCK_ENTRY_LEVEL3;
    }

    do {
      // Fill the Block Entry with attribute and output block address
      *BlockEntry &= BlockEntryMask;
      *BlockEntry |= (RegionStart & TT_ADDRESS_MASK_BLOCK_ENTRY) | Attributes | Type;

      // Go to the next BlockEntry
      RegionStart += BlockEntrySize;
      RegionLength -= BlockEntrySize;
      BlockEntry++;

      // Break the inner loop when next block is a table
      // Rerun GetBlockEntryListFromAddress to avoid page table memory leak
      if (TableLevel != 3 &&
          (*BlockEntry & TT_TYPE_MASK) == TT_TYPE_TABLE_ENTRY) {
            break;
      }
    } while ((RegionLength >= BlockEntrySize) && (BlockEntry <= LastBlockEntry));
  } while (RegionLength != 0);

  return RETURN_SUCCESS;
}

STATIC
RETURN_STATUS
FillTranslationTable (
  IN  UINT64                        *RootTable,
  IN  ARM_MEMORY_REGION_DESCRIPTOR  *MemoryRegion
  )
{
  return UpdateRegionMapping (
           RootTable,
           MemoryRegion->VirtualBase,
           MemoryRegion->Length,
           ArmMemoryAttributeToPageAttribute (MemoryRegion->Attributes) | TT_AF,
           0
           );
}

RETURN_STATUS
SetMemoryAttributes (
  IN EFI_PHYSICAL_ADDRESS      BaseAddress,
  IN UINT64                    Length,
  IN UINT64                    Attributes,
  IN EFI_PHYSICAL_ADDRESS      VirtualMask
  )
{
  RETURN_STATUS                Status;
  ARM_MEMORY_REGION_DESCRIPTOR MemoryRegion;
  UINT64                      *TranslationTable;

  MemoryRegion.PhysicalBase = BaseAddress;
  MemoryRegion.VirtualBase = BaseAddress;
  MemoryRegion.Length = Length;
  MemoryRegion.Attributes = GcdAttributeToArmAttribute (Attributes);

  TranslationTable = ArmGetTTBR0BaseAddress ();

  Status = FillTranslationTable (TranslationTable, &MemoryRegion);
  if (RETURN_ERROR (Status)) {
    return Status;
  }

  // Invalidate all TLB entries so changes are synced
  ArmInvalidateTlb ();

  return RETURN_SUCCESS;
}

STATIC
RETURN_STATUS
SetMemoryRegionAttribute (
  IN  EFI_PHYSICAL_ADDRESS      BaseAddress,
  IN  UINT64                    Length,
  IN  UINT64                    Attributes,
  IN  UINT64                    BlockEntryMask
  )
{
  RETURN_STATUS                Status;
  UINT64                       *RootTable;

  RootTable = ArmGetTTBR0BaseAddress ();

  Status = UpdateRegionMapping (RootTable, BaseAddress, Length, Attributes, BlockEntryMask);
  if (RETURN_ERROR (Status)) {
    return Status;
  }

  // Invalidate all TLB entries so changes are synced
  ArmInvalidateTlb ();

  return RETURN_SUCCESS;
}

RETURN_STATUS
ArmSetMemoryRegionNoExec (
  IN  EFI_PHYSICAL_ADDRESS      BaseAddress,
  IN  UINT64                    Length
  )
{
  UINT64    Val;

  if (ArmReadCurrentEL () == AARCH64_EL1) {
    Val = TT_PXN_MASK | TT_UXN_MASK;
  } else {
    Val = TT_XN_MASK;
  }

  return SetMemoryRegionAttribute (
           BaseAddress,
           Length,
           Val,
           ~TT_ADDRESS_MASK_BLOCK_ENTRY);
}

RETURN_STATUS
ArmClearMemoryRegionNoExec (
  IN  EFI_PHYSICAL_ADDRESS      BaseAddress,
  IN  UINT64                    Length
  )
{
  UINT64 Mask;

  // XN maps to UXN in the EL1&0 translation regime
  Mask = ~(TT_ADDRESS_MASK_BLOCK_ENTRY | TT_PXN_MASK | TT_XN_MASK);

  return SetMemoryRegionAttribute (
           BaseAddress,
           Length,
           0,
           Mask);
}

RETURN_STATUS
ArmSetMemoryRegionReadOnly (
  IN  EFI_PHYSICAL_ADDRESS      BaseAddress,
  IN  UINT64                    Length
  )
{
  return SetMemoryRegionAttribute (
           BaseAddress,
           Length,
           TT_AP_RO_RO,
           ~TT_ADDRESS_MASK_BLOCK_ENTRY);
}

RETURN_STATUS
ArmClearMemoryRegionReadOnly (
  IN  EFI_PHYSICAL_ADDRESS      BaseAddress,
  IN  UINT64                    Length
  )
{
  return SetMemoryRegionAttribute (
           BaseAddress,
           Length,
           TT_AP_RW_RW,
           ~(TT_ADDRESS_MASK_BLOCK_ENTRY | TT_AP_MASK));
}

RETURN_STATUS
EFIAPI
ArmConfigureMmu (
  IN  ARM_MEMORY_REGION_DESCRIPTOR  *MemoryTable,
  OUT VOID                         **TranslationTableBase OPTIONAL,
  OUT UINTN                         *TranslationTableSize OPTIONAL
  )
{
  VOID*                         TranslationTable;
  VOID*                         TranslationTableBuffer;
  UINT32                        TranslationTableAttribute;
  UINT64                        MaxAddress;
  UINTN                         T0SZ;
  UINTN                         RootTableEntryCount;
  UINTN                         RootTableEntrySize;
  UINT64                        TCR;
  RETURN_STATUS                 Status;

  if(MemoryTable == NULL) {
    ASSERT (MemoryTable != NULL);
    return RETURN_INVALID_PARAMETER;
  }

  // Cover the entire GCD memory space
  MaxAddress = (1UL << PcdGet8 (PcdPrePiCpuMemorySize)) - 1;

  // Lookup the Table Level to get the information
  LookupAddresstoRootTable (MaxAddress, &T0SZ, &RootTableEntryCount);

  //
  // Set TCR that allows us to retrieve T0SZ in the subsequent functions
  //
  // Ideally we will be running at EL2, but should support EL1 as well.
  // UEFI should not run at EL3.
  if (ArmReadCurrentEL () == AARCH64_EL2) {
    //Note: Bits 23 and 31 are reserved(RES1) bits in TCR_EL2
    TCR = T0SZ | (1UL << 31) | (1UL << 23) | TCR_TG0_4KB;

    // Set the Physical Address Size using MaxAddress
    if (MaxAddress < SIZE_4GB) {
      TCR |= TCR_PS_4GB;
    } else if (MaxAddress < SIZE_64GB) {
      TCR |= TCR_PS_64GB;
    } else if (MaxAddress < SIZE_1TB) {
      TCR |= TCR_PS_1TB;
    } else if (MaxAddress < SIZE_4TB) {
      TCR |= TCR_PS_4TB;
    } else if (MaxAddress < SIZE_16TB) {
      TCR |= TCR_PS_16TB;
    } else if (MaxAddress < SIZE_256TB) {
      TCR |= TCR_PS_256TB;
    } else {
      DEBUG ((EFI_D_ERROR, "ArmConfigureMmu: The MaxAddress 0x%lX is not supported by this MMU configuration.\n", MaxAddress));
      ASSERT (0); // Bigger than 48-bit memory space are not supported
      return RETURN_UNSUPPORTED;
    }
  } else if (ArmReadCurrentEL () == AARCH64_EL1) {
    // Due to Cortex-A57 erratum #822227 we must set TG1[1] == 1, regardless of EPD1.
    TCR = T0SZ | TCR_TG0_4KB | TCR_TG1_4KB | TCR_EPD1;

    // Set the Physical Address Size using MaxAddress
    if (MaxAddress < SIZE_4GB) {
      TCR |= TCR_IPS_4GB;
    } else if (MaxAddress < SIZE_64GB) {
      TCR |= TCR_IPS_64GB;
    } else if (MaxAddress < SIZE_1TB) {
      TCR |= TCR_IPS_1TB;
    } else if (MaxAddress < SIZE_4TB) {
      TCR |= TCR_IPS_4TB;
    } else if (MaxAddress < SIZE_16TB) {
      TCR |= TCR_IPS_16TB;
    } else if (MaxAddress < SIZE_256TB) {
      TCR |= TCR_IPS_256TB;
    } else {
      DEBUG ((EFI_D_ERROR, "ArmConfigureMmu: The MaxAddress 0x%lX is not supported by this MMU configuration.\n", MaxAddress));
      ASSERT (0); // Bigger than 48-bit memory space are not supported
      return RETURN_UNSUPPORTED;
    }
  } else {
    ASSERT (0); // UEFI is only expected to run at EL2 and EL1, not EL3.
    return RETURN_UNSUPPORTED;
  }

  //
  // Translation table walks are always cache coherent on ARMv8-A, so cache
  // maintenance on page tables is never needed. Since there is a risk of
  // loss of coherency when using mismatched attributes, and given that memory
  // is mapped cacheable except for extraordinary cases (such as non-coherent
  // DMA), have the page table walker perform cached accesses as well, and
  // assert below that that matches the attributes we use for CPU accesses to
  // the region.
  //
  TCR |= TCR_SH_INNER_SHAREABLE |
         TCR_RGN_OUTER_WRITE_BACK_ALLOC |
         TCR_RGN_INNER_WRITE_BACK_ALLOC;

  // Set TCR
  ArmSetTCR (TCR);

  // Allocate pages for translation table. Pool allocations are 8 byte aligned,
  // but we may require a higher alignment based on the size of the root table.
  RootTableEntrySize = RootTableEntryCount * sizeof(UINT64);
  if (RootTableEntrySize < EFI_PAGE_SIZE / 2) {
    TranslationTableBuffer = AllocatePool (2 * RootTableEntrySize - 8);
    //
    // Naturally align the root table. Preserves possible NULL value
    //
    TranslationTable = (VOID *)((UINTN)(TranslationTableBuffer - 1) | (RootTableEntrySize - 1)) + 1;
  } else {
    TranslationTable = AllocatePages (1);
    TranslationTableBuffer = NULL;
  }
  if (TranslationTable == NULL) {
    return RETURN_OUT_OF_RESOURCES;
  }
  // We set TTBR0 just after allocating the table to retrieve its location from the subsequent
  // functions without needing to pass this value across the functions. The MMU is only enabled
  // after the translation tables are populated.
  ArmSetTTBR0 (TranslationTable);

  if (TranslationTableBase != NULL) {
    *TranslationTableBase = TranslationTable;
  }

  if (TranslationTableSize != NULL) {
    *TranslationTableSize = RootTableEntrySize;
  }

  ZeroMem (TranslationTable, RootTableEntrySize);

  // Disable MMU and caches. ArmDisableMmu() also invalidates the TLBs
  ArmDisableMmu ();
  ArmDisableDataCache ();
  ArmDisableInstructionCache ();

  // Make sure nothing sneaked into the cache
  ArmCleanInvalidateDataCache ();
  ArmInvalidateInstructionCache ();

  TranslationTableAttribute = TT_ATTR_INDX_INVALID;
  while (MemoryTable->Length != 0) {

    DEBUG_CODE_BEGIN ();
      // Find the memory attribute for the Translation Table
      if ((UINTN)TranslationTable >= MemoryTable->PhysicalBase &&
          (UINTN)TranslationTable + RootTableEntrySize <= MemoryTable->PhysicalBase +
                                                          MemoryTable->Length) {
        TranslationTableAttribute = MemoryTable->Attributes;
      }
    DEBUG_CODE_END ();

    Status = FillTranslationTable (TranslationTable, MemoryTable);
    if (RETURN_ERROR (Status)) {
      goto FREE_TRANSLATION_TABLE;
    }
    MemoryTable++;
  }

  ASSERT (TranslationTableAttribute == ARM_MEMORY_REGION_ATTRIBUTE_WRITE_BACK ||
          TranslationTableAttribute == ARM_MEMORY_REGION_ATTRIBUTE_NONSECURE_WRITE_BACK);

  ArmSetMAIR (MAIR_ATTR(TT_ATTR_INDX_DEVICE_MEMORY, MAIR_ATTR_DEVICE_MEMORY) |                      // mapped to EFI_MEMORY_UC
              MAIR_ATTR(TT_ATTR_INDX_MEMORY_NON_CACHEABLE, MAIR_ATTR_NORMAL_MEMORY_NON_CACHEABLE) | // mapped to EFI_MEMORY_WC
              MAIR_ATTR(TT_ATTR_INDX_MEMORY_WRITE_THROUGH, MAIR_ATTR_NORMAL_MEMORY_WRITE_THROUGH) | // mapped to EFI_MEMORY_WT
              MAIR_ATTR(TT_ATTR_INDX_MEMORY_WRITE_BACK, MAIR_ATTR_NORMAL_MEMORY_WRITE_BACK));       // mapped to EFI_MEMORY_WB

  ArmDisableAlignmentCheck ();
  ArmEnableInstructionCache ();
  ArmEnableDataCache ();

  ArmEnableMmu ();
  return RETURN_SUCCESS;

FREE_TRANSLATION_TABLE:
  if (TranslationTableBuffer != NULL) {
    FreePool (TranslationTableBuffer);
  } else {
    FreePages (TranslationTable, 1);
  }
  return Status;
}

RETURN_STATUS
EFIAPI
ArmMmuBaseLibConstructor (
  VOID
  )
{
  extern UINT32 ArmReplaceLiveTranslationEntrySize;

  //
  // The ArmReplaceLiveTranslationEntry () helper function may be invoked
  // with the MMU off so we have to ensure that it gets cleaned to the PoC
  //
  WriteBackDataCacheRange (ArmReplaceLiveTranslationEntry,
    ArmReplaceLiveTranslationEntrySize);

  return RETURN_SUCCESS;
}
