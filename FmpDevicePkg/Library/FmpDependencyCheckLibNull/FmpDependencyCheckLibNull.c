/** @file
  Supports Capsule Dependency Expression.

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <PiDxe.h>
#include <Library/FmpDependencyCheckLib.h>

/**
  Check dependency for firmware update.

  @param[in]   ImageTypeId         Image Type Id.
  @param[in]   Version             New version.
  @param[in]   Dependencies        The dependencies.
  @param[in]   DependenciesSize    Size of the dependencies
  @param[out]  IsSatisfied         Indicate the dependencies is satisfied or not.

  @retval  EFI_SUCCESS             Dependency Evaluation is successful.
  @retval  Others                  Dependency Evaluation fails with unexpected error.

**/
EFI_STATUS
EFIAPI
EvaluateFmpDependencies (
  IN CONST EFI_GUID                ImageTypeId,
  IN CONST UINT32                  Version,
  IN CONST EFI_FIRMWARE_IMAGE_DEP  *Dependencies,
  IN CONST UINT32                  DependenciesSize,
  OUT BOOLEAN                      *IsSatisfied
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Save dependency to Fmp device.

  @param[in]  Depex               Fmp dependency.
  @param[in]  DepexSize           Size, in bytes, of the Fmp dependency.

  @retval  EFI_SUCCESS            Save Fmp dependnecy succeeds.
  @retval  Others                 Save Fmp dependnecy failes.
**/
EFI_STATUS
EFIAPI
SaveFmpDependency (
  IN EFI_FIRMWARE_IMAGE_DEP  *Depex,
  IN UINT32                  DepexSize
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Get dependency from the Fmp device.

  @param[out]  DepexSize          Size, in bytes, of the dependency.

  @retval  The pointer to dependency.
  @retval  NULL

**/
EFI_FIRMWARE_IMAGE_DEP*
EFIAPI
GetFmpDependency (
  OUT UINT32                 *DepexSize
  )
{
  return NULL;
}
