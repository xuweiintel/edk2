/** @file
  Fmp Capsule Dependency support functions for Firmware Management Protocol based
  firmware updates.

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __FMP_DEPENDENCY_CHECK_LIB__
#define __FMP_DEPENDENCY_CHECK_LIB__

#include <PiDxe.h>
#include <Protocol/FirmwareManagement.h>

/**
  Check dependency for firmware update.

  @param[in]   ImageTypeId    Image Type Id.
  @param[in]   Version        New version.
  @param[in]   Dependencies   The dependencies.
  @param[in]   DepexSize      Size of the dependencies

  @retval  EFI_SUCCESS   Dependency Evaluation is successful.
  @retval  Others        Dependency Evaluation fails with unexpected error.

**/
BOOLEAN
EFIAPI
CheckFmpDependency (
  IN CONST EFI_GUID                ImageTypeId,
  IN CONST UINT32                  Version,
  IN CONST EFI_FIRMWARE_IMAGE_DEP  *Depex,
  IN CONST UINT32                  DepexSize
  );

/**
  Save dependency to Fmp device.

  @param[in]  Depex       Fmp dependency.
  @param[in]  DepexSize   Size, in bytes, of the Fmp dependency.

  @retval  EFI_SUCCESS       Save Fmp dependency succeeds.
  @retval  EFI_UNSUPPORTED   Save Fmp dependency is not supported.
  @retval  Others            Save Fmp dependency failes.

**/
EFI_STATUS
EFIAPI
SaveFmpDependency (
  IN EFI_FIRMWARE_IMAGE_DEP  *Depex,
  IN UINT32                  DepexSize
  );

/**
  Get dependency from the Fmp device.

  @param[out]  DepexSize   Size, in bytes, of the dependency.

  @retval  The pointer to dependency.
  @retval  NULL

**/
EFI_FIRMWARE_IMAGE_DEP*
EFIAPI
GetFmpDependency (
  OUT UINT32  *DepexSize
  );

#endif