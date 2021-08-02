/** @file

  Copyright (c) 2015-2017, Linaro. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __DW_USB_H__
#define __DW_USB_H__

//
// Protocol GUID
//
#define DW_USB_PROTOCOL_GUID { 0x109fa264, 0x7811, 0x4862, { 0xa9, 0x73, 0x4a, 0xb2, 0xef, 0x2e, 0xe2, 0xff }}

//
// Protocol interface structure
//
typedef struct _DW_USB_PROTOCOL  DW_USB_PROTOCOL;

#define USB_HOST_MODE                 0
#define USB_DEVICE_MODE               1
#define USB_CABLE_NOT_ATTACHED        2

#define LANG_LENGTH                   8
#define MANU_FACTURER_STRING_LENGTH   32
#define PRODUCT_STRING_LENGTH         32
#define SERIAL_STRING_LENGTH          17

typedef
EFI_STATUS
(EFIAPI *DW_USB_GET_LANG) (
  OUT CHAR16                           *Lang,
  OUT UINT8                            *Length
  );

typedef
EFI_STATUS
(EFIAPI *DW_USB_GET_MANU_FACTURER) (
  OUT CHAR16                           *ManuFacturer,
  OUT UINT8                            *Length
  );

typedef
EFI_STATUS
(EFIAPI *DW_USB_GET_PRODUCT) (
  OUT CHAR16                           *Product,
  OUT UINT8                            *Length
  );

typedef
EFI_STATUS
(EFIAPI *DW_USB_GET_SERIAL_NO) (
  OUT CHAR16                           *SerialNo,
  OUT UINT8                            *Length
  );

typedef
EFI_STATUS
(EFIAPI *DW_USB_PHY_INIT) (
  IN UINT8                             Mode
  );

struct _DW_USB_PROTOCOL {
  DW_USB_GET_LANG                      GetLang;
  DW_USB_GET_MANU_FACTURER             GetManuFacturer;
  DW_USB_GET_PRODUCT                   GetProduct;
  DW_USB_GET_SERIAL_NO                 GetSerialNo;
  DW_USB_PHY_INIT                      PhyInit;
};

extern EFI_GUID gDwUsbProtocolGuid;

#endif
