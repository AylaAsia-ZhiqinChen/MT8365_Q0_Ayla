/** @file

  Copyright (c) 2011 - 2013, Intel Corporation. All rights reserved.<BR>
  Copyright (C) 2013, Red Hat, Inc.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "Uefi.h"
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/QemuFwCfgLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include "QemuFwCfgLibInternal.h"


/**
  Reads an 8-bit I/O port fifo into a block of memory.

  Reads the 8-bit I/O fifo port specified by Port.

  The port is read Count times, and the read data is
  stored in the provided Buffer.

  This function must guarantee that all I/O read and write operations are
  serialized.

  If 8-bit I/O port operations are not supported, then ASSERT().

  @param  Port    The I/O port to read.
  @param  Count   The number of times to read I/O port.
  @param  Buffer  The buffer to store the read data into.

**/
VOID
EFIAPI
IoReadFifo8 (
  IN      UINTN                     Port,
  IN      UINTN                     Count,
  OUT     VOID                      *Buffer
  );

/**
  Writes an 8-bit I/O port fifo from a block of memory.

  Writes the 8-bit I/O fifo port specified by Port.

  The port is written Count times, and the data are obtained
  from the provided Buffer.

  This function must guarantee that all I/O read and write operations are
  serialized.

  If 8-bit I/O port operations are not supported, then ASSERT().

  @param  Port    The I/O port to read.
  @param  Count   The number of times to read I/O port.
  @param  Buffer  The buffer to store the read data into.

**/
VOID
EFIAPI
IoWriteFifo8 (
  IN      UINTN                     Port,
  IN      UINTN                     Count,
  OUT     VOID                      *Buffer
  );


/**
  Selects a firmware configuration item for reading.
  
  Following this call, any data read from this item will start from
  the beginning of the configuration item's data.

  @param[in] QemuFwCfgItem - Firmware Configuration item to read

**/
VOID
EFIAPI
QemuFwCfgSelectItem (
  IN FIRMWARE_CONFIG_ITEM   QemuFwCfgItem
  )
{
  DEBUG ((EFI_D_INFO, "Select Item: 0x%x\n", (UINT16)(UINTN) QemuFwCfgItem));
  IoWrite16 (0x510, (UINT16)(UINTN) QemuFwCfgItem);
}


/**
  Transfer an array of bytes using the DMA interface.

  @param[in]     Size    Size in bytes to transfer.
  @param[in,out] Buffer  Buffer to read data into or write data from. May be
                         NULL if Size is zero.
  @param[in]     Write   TRUE if writing to fw_cfg from Buffer, FALSE if
                         reading from fw_cfg into Buffer.
**/
VOID
InternalQemuFwCfgDmaBytes (
  IN     UINT32   Size,
  IN OUT VOID     *Buffer OPTIONAL,
  IN     BOOLEAN  Write
  )
{
  volatile FW_CFG_DMA_ACCESS Access;
  UINT32                     AccessHigh, AccessLow;
  UINT32                     Status;

  if (Size == 0) {
    return;
  }

  Access.Control = SwapBytes32 (
                    Write ? FW_CFG_DMA_CTL_WRITE : FW_CFG_DMA_CTL_READ
                    );
  Access.Length  = SwapBytes32 (Size);
  Access.Address = SwapBytes64 ((UINTN)Buffer);

  //
  // Delimit the transfer from (a) modifications to Access, (b) in case of a
  // write, from writes to Buffer by the caller.
  //
  MemoryFence ();

  //
  // Start the transfer.
  //
  AccessHigh = (UINT32)RShiftU64 ((UINTN)&Access, 32);
  AccessLow  = (UINT32)(UINTN)&Access;
  IoWrite32 (0x514, SwapBytes32 (AccessHigh));
  IoWrite32 (0x518, SwapBytes32 (AccessLow));

  //
  // Don't look at Access.Control before starting the transfer.
  //
  MemoryFence ();

  //
  // Wait for the transfer to complete.
  //
  do {
    Status = SwapBytes32 (Access.Control);
    ASSERT ((Status & FW_CFG_DMA_CTL_ERROR) == 0);
  } while (Status != 0);

  //
  // After a read, the caller will want to use Buffer.
  //
  MemoryFence ();
}


/**
  Reads firmware configuration bytes into a buffer

  @param[in] Size - Size in bytes to read
  @param[in] Buffer - Buffer to store data into  (OPTIONAL if Size is 0)

**/
VOID
EFIAPI
InternalQemuFwCfgReadBytes (
  IN UINTN                  Size,
  IN VOID                   *Buffer  OPTIONAL
  )
{
  if (InternalQemuFwCfgDmaIsAvailable () && Size <= MAX_UINT32) {
    InternalQemuFwCfgDmaBytes ((UINT32)Size, Buffer, FALSE);
    return;
  }
  IoReadFifo8 (0x511, Size, Buffer);
}


/**
  Reads firmware configuration bytes into a buffer

  If called multiple times, then the data read will
  continue at the offset of the firmware configuration
  item where the previous read ended.

  @param[in] Size - Size in bytes to read
  @param[in] Buffer - Buffer to store data into

**/
VOID
EFIAPI
QemuFwCfgReadBytes (
  IN UINTN                  Size,
  IN VOID                   *Buffer
  )
{
  if (InternalQemuFwCfgIsAvailable ()) {
    InternalQemuFwCfgReadBytes (Size, Buffer);
  } else {
    ZeroMem (Buffer, Size);
  }
}

/**
  Write firmware configuration bytes from a buffer

  If called multiple times, then the data written will
  continue at the offset of the firmware configuration
  item where the previous write ended.

  @param[in] Size - Size in bytes to write
  @param[in] Buffer - Buffer to read data from

**/
VOID
EFIAPI
QemuFwCfgWriteBytes (
  IN UINTN                  Size,
  IN VOID                   *Buffer
  )
{
  if (InternalQemuFwCfgIsAvailable ()) {
    if (InternalQemuFwCfgDmaIsAvailable () && Size <= MAX_UINT32) {
      InternalQemuFwCfgDmaBytes ((UINT32)Size, Buffer, TRUE);
      return;
    }
    IoWriteFifo8 (0x511, Size, Buffer);
  }
}


/**
  Reads a UINT8 firmware configuration value

  @return    Value of Firmware Configuration item read

**/
UINT8
EFIAPI
QemuFwCfgRead8 (
  VOID
  )
{
  UINT8 Result;

  QemuFwCfgReadBytes (sizeof (Result), &Result);

  return Result;
}


/**
  Reads a UINT16 firmware configuration value

  @return    Value of Firmware Configuration item read

**/
UINT16
EFIAPI
QemuFwCfgRead16 (
  VOID
  )
{
  UINT16 Result;

  QemuFwCfgReadBytes (sizeof (Result), &Result);

  return Result;
}


/**
  Reads a UINT32 firmware configuration value

  @return    Value of Firmware Configuration item read

**/
UINT32
EFIAPI
QemuFwCfgRead32 (
  VOID
  )
{
  UINT32 Result;

  QemuFwCfgReadBytes (sizeof (Result), &Result);

  return Result;
}


/**
  Reads a UINT64 firmware configuration value

  @return    Value of Firmware Configuration item read

**/
UINT64
EFIAPI
QemuFwCfgRead64 (
  VOID
  )
{
  UINT64 Result;

  QemuFwCfgReadBytes (sizeof (Result), &Result);

  return Result;
}


/**
  Find the configuration item corresponding to the firmware configuration file.

  @param[in]  Name - Name of file to look up.
  @param[out] Item - Configuration item corresponding to the file, to be passed
                     to QemuFwCfgSelectItem ().
  @param[out] Size - Number of bytes in the file.

  @return    RETURN_SUCCESS       If file is found.
             RETURN_NOT_FOUND     If file is not found.
             RETURN_UNSUPPORTED   If firmware configuration is unavailable.

**/
RETURN_STATUS
EFIAPI
QemuFwCfgFindFile (
  IN   CONST CHAR8           *Name,
  OUT  FIRMWARE_CONFIG_ITEM  *Item,
  OUT  UINTN                 *Size
  )
{
  UINT32 Count;
  UINT32 Idx;

  if (!InternalQemuFwCfgIsAvailable ()) {
    return RETURN_UNSUPPORTED;
  }

  QemuFwCfgSelectItem (QemuFwCfgItemFileDir);
  Count = SwapBytes32 (QemuFwCfgRead32 ());

  for (Idx = 0; Idx < Count; ++Idx) {
    UINT32 FileSize;
    UINT16 FileSelect;
    UINT16 FileReserved;
    CHAR8  FName[QEMU_FW_CFG_FNAME_SIZE];

    FileSize     = QemuFwCfgRead32 ();
    FileSelect   = QemuFwCfgRead16 ();
    FileReserved = QemuFwCfgRead16 ();
    (VOID) FileReserved; /* Force a do-nothing reference. */
    InternalQemuFwCfgReadBytes (sizeof (FName), FName);

    if (AsciiStrCmp (Name, FName) == 0) {
      *Item = SwapBytes16 (FileSelect);
      *Size = SwapBytes32 (FileSize);
      return RETURN_SUCCESS;
    }
  }

  return RETURN_NOT_FOUND;
}


/**
  Determine if S3 support is explicitly enabled.

  @retval  TRUE   if S3 support is explicitly enabled.
           FALSE  otherwise. This includes unavailability of the firmware
                  configuration interface.
**/
BOOLEAN
EFIAPI
QemuFwCfgS3Enabled (
  VOID
  )
{
  RETURN_STATUS        Status;
  FIRMWARE_CONFIG_ITEM FwCfgItem;
  UINTN                FwCfgSize;
  UINT8                SystemStates[6];

  Status = QemuFwCfgFindFile ("etc/system-states", &FwCfgItem, &FwCfgSize);
  if (Status != RETURN_SUCCESS || FwCfgSize != sizeof SystemStates) {
    return FALSE;
  }
  QemuFwCfgSelectItem (FwCfgItem);
  QemuFwCfgReadBytes (sizeof SystemStates, SystemStates);
  return (BOOLEAN) (SystemStates[3] & BIT7);
}
