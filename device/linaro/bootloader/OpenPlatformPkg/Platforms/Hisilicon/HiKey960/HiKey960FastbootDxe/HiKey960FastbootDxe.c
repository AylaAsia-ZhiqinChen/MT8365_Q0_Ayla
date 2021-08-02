/** @file

  Copyright (c) 2014, ARM Ltd. All rights reserved.<BR>
  Copyright (c) 2015-2017, Linaro. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

/*
  Implementation of the Android Fastboot Platform protocol, to be used by the
  Fastboot UEFI application, for Hisilicon HiKey platform.
*/

#include <Protocol/AndroidFastbootPlatform.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DiskIo.h>
#include <Protocol/EraseBlock.h>
#include <Protocol/SimpleTextOut.h>

#include <Protocol/DevicePathToText.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UsbSerialNumberLib.h>
#include <Library/PrintLib.h>
#include <Library/TimerLib.h>

#define PARTITION_NAME_MAX_LENGTH        (72/2)

#define SERIAL_NUMBER_LBA                20
#define RANDOM_MAX                       0x7FFFFFFFFFFFFFFF
#define RANDOM_MAGIC                     0x9A4DBEAF

#define ADB_REBOOT_ADDRESS               0x32100000
#define ADB_REBOOT_BOOTLOADER            0x77665500

#define UFS_BLOCK_SIZE                   4096

typedef struct _FASTBOOT_PARTITION_LIST {
  LIST_ENTRY  Link;
  CHAR16      PartitionName[PARTITION_NAME_MAX_LENGTH];
  EFI_LBA     StartingLBA;
  EFI_LBA     EndingLBA;
} FASTBOOT_PARTITION_LIST;

STATIC LIST_ENTRY                       mPartitionListHead;
STATIC EFI_HANDLE                       mFlashHandle;
STATIC EFI_BLOCK_IO_PROTOCOL           *mFlashBlockIo;
STATIC EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *mTextOut;

/*
  Helper to free the partition list
*/
STATIC
VOID
FreePartitionList (
  VOID
  )
{
  FASTBOOT_PARTITION_LIST *Entry;
  FASTBOOT_PARTITION_LIST *NextEntry;

  Entry = (FASTBOOT_PARTITION_LIST *) GetFirstNode (&mPartitionListHead);
  while (!IsNull (&mPartitionListHead, &Entry->Link)) {
    NextEntry = (FASTBOOT_PARTITION_LIST *) GetNextNode (&mPartitionListHead, &Entry->Link);

    RemoveEntryList (&Entry->Link);
    FreePool (Entry);

    Entry = NextEntry;
  }
}

/*
  Read the PartitionName fields from the GPT partition entries, putting them
  into an allocated array that should later be freed.
*/
STATIC
EFI_STATUS
ReadPartitionEntries (
  IN  EFI_BLOCK_IO_PROTOCOL *BlockIo,
  OUT EFI_PARTITION_ENTRY  **PartitionEntries,
  OUT UINTN                 *PartitionNumbers
  )
{
  UINT32                      MediaId;
  EFI_PARTITION_TABLE_HEADER *GptHeader;
  EFI_PARTITION_ENTRY        *Entry;
  EFI_STATUS                  Status;
  VOID                       *Buffer;
  UINTN                       PageCount;
  UINTN                       BlockSize;
  UINTN                       Count, EndLBA;

  if ((PartitionEntries == NULL) || (PartitionNumbers == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  MediaId = BlockIo->Media->MediaId;
  BlockSize = BlockIo->Media->BlockSize;

  //
  // Read size of Partition entry and number of entries from GPT header
  //

  PageCount = EFI_SIZE_TO_PAGES (6 * BlockSize);
  Buffer = AllocatePages (PageCount);
  if (Buffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = BlockIo->ReadBlocks (BlockIo, MediaId, 0, PageCount * EFI_PAGE_SIZE, Buffer);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  GptHeader = (EFI_PARTITION_TABLE_HEADER *)(Buffer + BlockSize);

  // Check there is a GPT on the media
  if (GptHeader->Header.Signature != EFI_PTAB_HEADER_ID ||
      GptHeader->MyLBA != 1) {
    DEBUG ((DEBUG_ERROR,
      "Fastboot platform: No GPT on flash. "
      "Fastboot on Versatile Express does not support MBR.\n"
      ));
    return EFI_DEVICE_ERROR;
  }

  Entry = (EFI_PARTITION_ENTRY *)(Buffer + (2 * BlockSize));
  EndLBA = GptHeader->FirstUsableLBA - 1;
  Count = 0;
  while (1) {
    if ((Entry->StartingLBA > EndLBA) && (Entry->EndingLBA <= GptHeader->LastUsableLBA)) {
      Count++;
      EndLBA = Entry->EndingLBA;
      Entry++;
    } else {
      break;
    }
  }
  if (Count == 0) {
    return EFI_INVALID_PARAMETER;
  }
  if (Count > GptHeader->NumberOfPartitionEntries) {
    Count = GptHeader->NumberOfPartitionEntries;
  }

  *PartitionEntries = (EFI_PARTITION_ENTRY *)((UINTN)Buffer + (2 * BlockSize));
  *PartitionNumbers = Count;
  return EFI_SUCCESS;
}

EFI_STATUS
LoadPtable (
  VOID
  )
{
  EFI_STATUS                          Status;
  EFI_DEVICE_PATH_PROTOCOL           *FlashDevicePath;
  EFI_DEVICE_PATH_PROTOCOL           *FlashDevicePathDup;
  UINTN                               PartitionNumbers = 0;
  UINTN                               LoopIndex;
  EFI_PARTITION_ENTRY                *PartitionEntries = NULL;
  FASTBOOT_PARTITION_LIST            *Entry;

  InitializeListHead (&mPartitionListHead);

  Status = gBS->LocateProtocol (&gEfiSimpleTextOutProtocolGuid, NULL, (VOID **) &mTextOut);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR,
      "Fastboot platform: Couldn't open Text Output Protocol: %r\n", Status
      ));
    return Status;
  }

  //
  // Get EFI_HANDLES for all the partitions on the block devices pointed to by
  // PcdFastbootFlashDevicePath, also saving their GPT partition labels.
  // There's no way to find all of a device's children, so we get every handle
  // in the system supporting EFI_BLOCK_IO_PROTOCOL and then filter out ones
  // that don't represent partitions on the flash device.
  //
  FlashDevicePath = ConvertTextToDevicePath ((CHAR16*)FixedPcdGetPtr (PcdAndroidFastbootNvmDevicePath));

  // Create another device path pointer because LocateDevicePath will modify it.
  FlashDevicePathDup = FlashDevicePath;
  Status = gBS->LocateDevicePath (&gEfiBlockIoProtocolGuid, &FlashDevicePathDup, &mFlashHandle);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Warning: Couldn't locate Android NVM device (status: %r)\n", Status));
    // Failing to locate partitions should not prevent to do other Android FastBoot actions
    return EFI_SUCCESS;
  }


  Status = gBS->OpenProtocol (
                  mFlashHandle,
                  &gEfiBlockIoProtocolGuid,
                  (VOID **) &mFlashBlockIo,
                  gImageHandle,
                  NULL,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Fastboot platform: Couldn't open Android NVM device (status: %r)\n", Status));
    return EFI_DEVICE_ERROR;
  }

  // Read the GPT partition entry array into memory so we can get the partition names
  Status = ReadPartitionEntries (mFlashBlockIo, &PartitionEntries, &PartitionNumbers);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Warning: Failed to read partitions from Android NVM device (status: %r)\n", Status));
    // Failing to locate partitions should not prevent to do other Android FastBoot actions
    return EFI_SUCCESS;
  }
  for (LoopIndex = 0; LoopIndex < PartitionNumbers; LoopIndex++) {
    // Create entry
    Entry = AllocatePool (sizeof (FASTBOOT_PARTITION_LIST));
    if (Entry == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      FreePartitionList ();
      goto Exit;
    }
    StrnCpy (
      Entry->PartitionName,
      PartitionEntries[LoopIndex].PartitionName,
      PARTITION_NAME_MAX_LENGTH
      );
    Entry->StartingLBA = PartitionEntries[LoopIndex].StartingLBA;
    Entry->EndingLBA = PartitionEntries[LoopIndex].EndingLBA;
    InsertTailList (&mPartitionListHead, &Entry->Link);
  }
Exit:
  FreePages (
    (VOID *)((UINTN)PartitionEntries - (2 * mFlashBlockIo->Media->BlockSize)),
    EFI_SIZE_TO_PAGES (6 * mFlashBlockIo->Media->BlockSize)
    );
  return Status;
}

/*
  Initialise: Open the Android NVM device and find the partitions on it. Save them in
  a list along with the "PartitionName" fields for their GPT entries.
  We will use these partition names as the key in
  HiKey960FastbootPlatformFlashPartition.
*/
EFI_STATUS
HiKey960FastbootPlatformInit (
  VOID
  )
{
  EFI_STATUS               Status;

  Status = LoadPtable ();
  if (EFI_ERROR (Status)) {
    return Status;
  }
  return Status;
}

VOID
HiKey960FastbootPlatformUnInit (
  VOID
  )
{
  FreePartitionList ();
}

EFI_STATUS
HiKey960FlashPtable (
  IN UINTN   Size,
  IN VOID   *Image
  )
{
  EFI_STATUS               Status;
  Status = mFlashBlockIo->WriteBlocks (
                            mFlashBlockIo,
                            mFlashBlockIo->Media->MediaId,
                            0,
                            Size,
                            Image
                            );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to write (status:%r)\n", Status));
    return Status;
  }
  FreePartitionList ();
  Status = LoadPtable ();
  return Status;
}

EFI_STATUS
HiKey960ErasePtable (
  VOID
  )
{
  EFI_STATUS                  Status;
#if 0
  EFI_ERASE_BLOCK_PROTOCOL   *EraseBlockProtocol;
#endif

#if 0
  Status = gBS->OpenProtocol (
                  mFlashHandle,
                  &gEfiEraseBlockProtocolGuid,
                  (VOID **) &EraseBlockProtocol,
                  gImageHandle,
                  NULL,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Fastboot platform: could not open Erase Block IO: %r\n", Status));
    return EFI_DEVICE_ERROR;
  }
  Status = EraseBlockProtocol->EraseBlocks (
                                 EraseBlockProtocol,
                                 mFlashBlockIo->Media->MediaId,
                                 0,
                                 NULL,
                                 6 * mFlashBlockIo->Media->BlockSize
                                 );
#else
  {
    VOID         *DataPtr;
    UINTN         Lba;

    DataPtr = AllocatePages (1);
    ZeroMem (DataPtr, EFI_PAGE_SIZE);
    for (Lba = 0; Lba < 6; Lba++) {
      Status = mFlashBlockIo->WriteBlocks (
                          mFlashBlockIo,
                          mFlashBlockIo->Media->MediaId,
                          Lba,
                          EFI_PAGE_SIZE,
                          DataPtr
                          );
      if (EFI_ERROR (Status)) {
        goto Exit;
      }
    }
Exit:
    FreePages (DataPtr, 1);
  }
#endif
  FreePartitionList ();
  return Status;
}

EFI_STATUS
HiKey960FlashXloader (
  IN UINTN   Size,
  IN VOID   *Image
  )
{
  EFI_STATUS                        Status;
  EFI_DEVICE_PATH_PROTOCOL         *DevicePath;
  EFI_HANDLE                        Handle;
  EFI_BLOCK_IO_PROTOCOL            *BlockIo;
  EFI_DISK_IO_PROTOCOL             *DiskIo;

  DevicePath = ConvertTextToDevicePath ((CHAR16*)FixedPcdGetPtr (PcdXloaderDevicePath));

  Status = gBS->LocateDevicePath (&gEfiBlockIoProtocolGuid, &DevicePath, &Handle);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Warning: Couldn't locate xloader device (status: %r)\n", Status));
    return Status;
  }

  Status = gBS->OpenProtocol (
                  Handle,
                  &gEfiBlockIoProtocolGuid,
                  (VOID **) &BlockIo,
                  gImageHandle,
                  NULL,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Fastboot platform: Couldn't open xloader device (status: %r)\n", Status));
    return EFI_DEVICE_ERROR;
  }
  Status = gBS->OpenProtocol (
                  Handle,
                  &gEfiDiskIoProtocolGuid,
                  (VOID **) &DiskIo,
                  gImageHandle,
                  NULL,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = DiskIo->WriteDisk (
                     DiskIo,
                     BlockIo->Media->MediaId,
                     0,
                     Size,
                     Image
                     );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to write (status:%r)\n", Status));
    return Status;
  }
  return Status;
}

EFI_STATUS
HiKey960FastbootPlatformFlashPartition (
  IN CHAR8  *PartitionName,
  IN UINTN   Size,
  IN VOID   *Image
  )
{
  EFI_STATUS               Status;
  UINTN                    PartitionSize;
  FASTBOOT_PARTITION_LIST *Entry;
  CHAR16                   PartitionNameUnicode[60];
  BOOLEAN                  PartitionFound;
  EFI_DISK_IO_PROTOCOL    *DiskIo;
  UINTN                    BlockSize;

  // Support the pseudo partition name, such as "ptable".
  if (AsciiStrCmp (PartitionName, "ptable") == 0) {
    return HiKey960FlashPtable (Size, Image);
  } else if (AsciiStrCmp (PartitionName, "xloader") == 0) {
    return HiKey960FlashXloader (Size, Image);
  }

  AsciiStrToUnicodeStr (PartitionName, PartitionNameUnicode);
  PartitionFound = FALSE;
  Entry = (FASTBOOT_PARTITION_LIST *) GetFirstNode (&(mPartitionListHead));
  while (!IsNull (&mPartitionListHead, &Entry->Link)) {
    // Search the partition list for the partition named by PartitionName
    if (StrCmp (Entry->PartitionName, PartitionNameUnicode) == 0) {
      PartitionFound = TRUE;
      break;
    }

   Entry = (FASTBOOT_PARTITION_LIST *) GetNextNode (&mPartitionListHead, &(Entry)->Link);
  }
  if (!PartitionFound) {
    return EFI_NOT_FOUND;
  }

  // Check image will fit on device
  BlockSize = mFlashBlockIo->Media->BlockSize;
  PartitionSize = (Entry->EndingLBA - Entry->StartingLBA + 1) * BlockSize;
  if (PartitionSize < Size) {
    DEBUG ((DEBUG_ERROR, "Partition not big enough.\n"));
    DEBUG ((DEBUG_ERROR, "Partition Size:\t%ld\nImage Size:\t%ld\n", PartitionSize, Size));

    return EFI_VOLUME_FULL;
  }

  Status = gBS->OpenProtocol (
                  mFlashHandle,
                  &gEfiDiskIoProtocolGuid,
                  (VOID **) &DiskIo,
                  gImageHandle,
                  NULL,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  ASSERT_EFI_ERROR (Status);

  Status = DiskIo->WriteDisk (
                     DiskIo,
                     mFlashBlockIo->Media->MediaId,
                     Entry->StartingLBA * BlockSize,
                     Size,
                     Image
                     );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to write %d bytes into 0x%x, Status:%r\n", Size, Entry->StartingLBA * BlockSize, Status));
    return Status;
  }

  mFlashBlockIo->FlushBlocks(mFlashBlockIo);
  MicroSecondDelay (50000);

  return Status;
}

EFI_STATUS
HiKey960FastbootPlatformErasePartition (
  IN CHAR8 *PartitionName
  )
{
  EFI_STATUS                  Status;
  EFI_ERASE_BLOCK_PROTOCOL   *EraseBlockProtocol;
  UINTN                       Size;
  BOOLEAN                     PartitionFound;
  CHAR16                      PartitionNameUnicode[60];
  FASTBOOT_PARTITION_LIST    *Entry;

  AsciiStrToUnicodeStr (PartitionName, PartitionNameUnicode);

  // Support the pseudo partition name, such as "ptable".
  if (AsciiStrCmp (PartitionName, "ptable") == 0) {
    return HiKey960ErasePtable ();
  }

  PartitionFound = FALSE;
  Entry = (FASTBOOT_PARTITION_LIST *) GetFirstNode (&mPartitionListHead);
  while (!IsNull (&mPartitionListHead, &Entry->Link)) {
    // Search the partition list for the partition named by PartitionName
    if (StrCmp (Entry->PartitionName, PartitionNameUnicode) == 0) {
      PartitionFound = TRUE;
      break;
    }
    Entry = (FASTBOOT_PARTITION_LIST *) GetNextNode (&mPartitionListHead, &Entry->Link);
  }
  if (!PartitionFound) {
    return EFI_NOT_FOUND;
  }

  Status = gBS->OpenProtocol (
                  mFlashHandle,
                  &gEfiEraseBlockProtocolGuid,
                  (VOID **) &EraseBlockProtocol,
                  gImageHandle,
                  NULL,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Size = (Entry->EndingLBA - Entry->StartingLBA + 1) * mFlashBlockIo->Media->BlockSize;
  Status = EraseBlockProtocol->EraseBlocks (
                                 EraseBlockProtocol,
                                 mFlashBlockIo->Media->MediaId,
                                 Entry->StartingLBA,
                                 NULL,
                                 Size
                                 );
  return Status;
}

EFI_STATUS
HiKey960FastbootPlatformGetVar (
  IN  CHAR8   *Name,
  OUT CHAR8   *Value
  )
{
  EFI_STATUS               Status = EFI_SUCCESS;
  UINT64                   PartitionSize;
  FASTBOOT_PARTITION_LIST *Entry;
  CHAR16                   PartitionNameUnicode[60];
  BOOLEAN                  PartitionFound;
  CHAR16                   UnicodeSN[SERIAL_NUMBER_SIZE];

  if (!AsciiStrCmp (Name, "max-download-size")) {
    AsciiStrCpy (Value, FixedPcdGetPtr (PcdArmFastbootFlashLimit));
  } else if (!AsciiStrCmp (Name, "product")) {
    AsciiStrCpy (Value, FixedPcdGetPtr (PcdFirmwareVendor));
  } else if (!AsciiStrCmp (Name, "serialno")) {
    Status = LoadSNFromBlock (mFlashHandle, SERIAL_NUMBER_LBA, UnicodeSN);
    UnicodeStrToAsciiStr (UnicodeSN, Value);
  } else if ( !AsciiStrnCmp (Name, "partition-size", 14)) {
    AsciiStrToUnicodeStr ((Name + 15), PartitionNameUnicode);
    PartitionFound = FALSE;
    Entry = (FASTBOOT_PARTITION_LIST *) GetFirstNode (&(mPartitionListHead));
    while (!IsNull (&mPartitionListHead, &Entry->Link)) {
      // Search the partition list for the partition named by PartitionName
      if (StrCmp (Entry->PartitionName, PartitionNameUnicode) == 0) {
        PartitionFound = TRUE;
        break;
      }

     Entry = (FASTBOOT_PARTITION_LIST *) GetNextNode (&mPartitionListHead, &(Entry)->Link);
    }
    if (!PartitionFound) {
      *Value = '\0';
      return EFI_NOT_FOUND;
    }

    PartitionSize = (Entry->EndingLBA - Entry->StartingLBA + 1) * mFlashBlockIo->Media->BlockSize;
    DEBUG ((DEBUG_ERROR, "Fastboot platform: check for partition-size:%a 0X%llx\n", Name, PartitionSize ));
    AsciiSPrint (Value, 12, "0x%llx", PartitionSize);
  } else if ( !AsciiStrnCmp (Name, "partition-type", 14)) {
      DEBUG ((DEBUG_ERROR, "Fastboot platform: check for partition-type:%a\n", (Name + 15) ));
    if ( !AsciiStrnCmp  ( (Name + 15) , "system", 6) || !AsciiStrnCmp  ( (Name + 15) , "userdata", 8)
            || !AsciiStrnCmp  ( (Name + 15) , "cache", 5)) {
      AsciiStrCpy (Value, "ext4");
    } else {
      AsciiStrCpy (Value, "raw");
    }
  } else if ( !AsciiStrCmp (Name, "erase-block-size")) {
    AsciiSPrint (Value, 12, "0x%llx", UFS_BLOCK_SIZE);
  } else if ( !AsciiStrCmp (Name, "logical-block-size")) {
    AsciiSPrint (Value, 12, "0x%llx", UFS_BLOCK_SIZE);
  } else {
    *Value = '\0';
  }
  return Status;
}

EFI_STATUS
HiKey960FastbootPlatformOemCommand (
  IN  CHAR8   *Command
  )
{
  EFI_STATUS   Status;
  CHAR16       UnicodeSN[SERIAL_NUMBER_SIZE];
  UINTN        Size;

  Size = AsciiStrLen ("serialno");
  if (AsciiStrCmp (Command, "Demonstrate") == 0) {
    DEBUG ((DEBUG_ERROR, "ARM OEM Fastboot command 'Demonstrate' received.\n"));
    return EFI_SUCCESS;
  } else if (AsciiStrnCmp (Command, "serialno", Size) == 0) {
    while (*(Command + Size) == ' ') {
      Size++;
    }
    if (AsciiStrnCmp (Command + Size, "set", AsciiStrLen ("set")) == 0) {
      Size += AsciiStrLen ("set");
      while (*(Command + Size) == ' ') {
        Size++;
      }
      Status = AssignUsbSN (Command + Size, UnicodeSN);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "Failed to set USB Serial Number.\n"));
        return Status;
      }
    } else {
      Status = GenerateUsbSN (UnicodeSN);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "Failed to generate USB Serial Number.\n"));
        return Status;
      }
    }
    Status = StoreSNToBlock (mFlashHandle, SERIAL_NUMBER_LBA, UnicodeSN);
    return Status;
  } else if (AsciiStrCmp (Command, "reboot-bootloader") == 0) {
    MmioWrite32 (ADB_REBOOT_ADDRESS, ADB_REBOOT_BOOTLOADER);
    WriteBackInvalidateDataCacheRange ((VOID *)ADB_REBOOT_ADDRESS, 4);
    return EFI_SUCCESS;
  } else {
    DEBUG ((DEBUG_ERROR,
      "HiKey960: Unrecognised Fastboot OEM command: %a\n",
      Command
      ));
    return EFI_NOT_FOUND;
  }
}

EFI_STATUS
HiKey960FastbootPlatformFlashPartitionEx (
  IN CHAR8  *PartitionName,
  IN UINTN   Offset,
  IN UINTN   Size,
  IN VOID   *Image
  )
{
  EFI_STATUS               Status;
  UINTN                    PartitionSize;
  FASTBOOT_PARTITION_LIST *Entry;
  CHAR16                   PartitionNameUnicode[60];
  BOOLEAN                  PartitionFound;
  UINTN                    BlockSize;
  EFI_DISK_IO_PROTOCOL    *DiskIo;

  AsciiStrToUnicodeStr (PartitionName, PartitionNameUnicode);
  PartitionFound = FALSE;
  Entry = (FASTBOOT_PARTITION_LIST *) GetFirstNode (&(mPartitionListHead));
  while (!IsNull (&mPartitionListHead, &Entry->Link)) {
    // Search the partition list for the partition named by PartitionName
    if (StrCmp (Entry->PartitionName, PartitionNameUnicode) == 0) {
      PartitionFound = TRUE;
      break;
    }

   Entry = (FASTBOOT_PARTITION_LIST *) GetNextNode (&mPartitionListHead, &(Entry)->Link);
  }
  if (!PartitionFound) {
    return EFI_NOT_FOUND;
  }

  // Check image will fit on device
  PartitionSize = (Entry->EndingLBA - Entry->StartingLBA + 1) * mFlashBlockIo->Media->BlockSize;
  if (PartitionSize < Size) {
    DEBUG ((DEBUG_ERROR, "Partition not big enough.\n"));
    DEBUG ((DEBUG_ERROR, "Partition Size:\t%ld\nImage Size:\t%ld\n", PartitionSize, Size));

    return EFI_VOLUME_FULL;
  }

  BlockSize = mFlashBlockIo->Media->BlockSize;

  Status = gBS->OpenProtocol (
                  mFlashHandle,
                  &gEfiDiskIoProtocolGuid,
                  (VOID **) &DiskIo,
                  gImageHandle,
                  NULL,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = DiskIo->WriteDisk (
                     DiskIo,
                     mFlashBlockIo->Media->MediaId,
                     Entry->StartingLBA * BlockSize + Offset,
                     Size,
                     Image
                     );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to write %d bytes into 0x%x, Status:%r\n", Size, Entry->StartingLBA * BlockSize + Offset, Status));
    return Status;
  }
  return Status;
}

FASTBOOT_PLATFORM_PROTOCOL mPlatformProtocol = {
  HiKey960FastbootPlatformInit,
  HiKey960FastbootPlatformUnInit,
  HiKey960FastbootPlatformFlashPartition,
  HiKey960FastbootPlatformErasePartition,
  HiKey960FastbootPlatformGetVar,
  HiKey960FastbootPlatformOemCommand,
  HiKey960FastbootPlatformFlashPartitionEx
};

EFI_STATUS
EFIAPI
HiKey960FastbootPlatformEntryPoint (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
  )
{
  return gBS->InstallProtocolInterface (
                &ImageHandle,
                &gAndroidFastbootPlatformProtocolGuid,
                EFI_NATIVE_INTERFACE,
                &mPlatformProtocol
                );
}
