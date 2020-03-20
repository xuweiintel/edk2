/** @file
  Fmp Capsule Dependency support functions for Firmware Management Protocol based
  firmware updates.

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __FMP_DEPENDENCY_LIB__
#define __FMP_DEPENDENCY_LIB__

#include <PiDxe.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Protocol/FirmwareManagement.h>


EFIAPI
EFI_FIRMWARE_IMAGE_DEP*
RetrieveDepexFromCap (
  IN  EFI_FIRMWARE_IMAGE_AUTHENTICATION *Image,
  IN  UINTN                             ImageSize,
  OUT UINTN                             *DepexSize
);

EFI_STATUS
EvaluateDependencies (
  IN CONST EFI_GUID                ImageTypeId,
  IN CONST UINT32                  Version,
  IN CONST EFI_FIRMWARE_IMAGE_DEP  *Depex,
  IN CONST UINT32                  DepexSize,
  OUT BOOLEAN                      *IsSatisfied
  );

EFIAPI
EFI_STATUS
StoreDepexToDevice (
  IN EFI_FIRMWARE_IMAGE_DEP* Depex,
  IN UINTN                   DepexSize
);

EFIAPI
EFI_FIRMWARE_IMAGE_DEP*
RetrieveDepexFromDevice (
  OUT UINTN  *DepexSize
);

#endif