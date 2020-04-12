/** @file
  Fmp Capsule Dependency support functions for Firmware Management Protocol based
  firmware updates.

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __FMP_DEPENDENCY_LIB__
#define __FMP_DEPENDENCY_LIB__

#include <PiDxe.h>
#include <Protocol/FirmwareManagement.h>

//
// Data struct to store FMP ImageType and version for dependency check. 
//
typedef struct {
  EFI_GUID ImageTypeId;
  UINT32   Version;
} FMP_DEPEX_CHECK_VERSION_DATA;

/**
  Validate the dependency expression and output its size.

  @param[in]   ImageDepex      Pointer to the EFI_FIRMWARE_IMAGE_DEP.
  @param[in]   MaxDepexSize    Max size of the dependency.
  @param[out]  DepexSize       Size of dependency.

  @retval TRUE           The capsule is valid.
  @retval FALSE          The capsule is invalid.

**/
BOOLEAN
ValidateDependency (
  IN  EFI_FIRMWARE_IMAGE_DEP             *ImageDepex,
  IN  CONST UINTN                        MaxDepexSize,
  OUT UINT32                             *DepexSize
  );

/**
  Get the size of dependencies. Assume the dependencies is validated before
  calling this function.

  @param[in]   Dependencies    Pointer to the EFI_FIRMWARE_IMAGE_DEP.

  @retval  The size of dependencies.

**/
UINTN
EFIAPI
GetDependencySize (
  IN CONST EFI_FIRMWARE_IMAGE_DEP  *Dependencies
  );

/**
  Get dependency from the new capsule.

  @param[in]  Image             Points to the new firmware image.
  @param[in]  ImageSize         Size, in bytes, of the new firmware image.
  @param[out] DepexSize         Size, in bytes, of the dependency.

  @retval  The pointer to dependency.
  @retval  Null 

**/
EFI_FIRMWARE_IMAGE_DEP*
EFIAPI
GetImageDependency (
  IN  EFI_FIRMWARE_IMAGE_AUTHENTICATION *Image,
  IN  UINTN                             ImageSize,
  OUT UINT32                            *DepexSize
  );

/**
  Evaluate the dependencies.

  @param[in]   Dependencies        Dependency expressions.
  @param[in]   DependenciesSize    Size of Dependency expressions.
  @param[in]   FmpVersions         Array of Fmp ImageTypeId and version.
  @param[in]   FmpVersionsCount    Element count of the arrary

  @retval TRUE    Dependency expressions evaluate to TRUE.
  @retval FALSE   Dependency expressions evaluate to FALSE.

**/
BOOLEAN
EFIAPI
EvaluateDependency (
  IN CONST EFI_FIRMWARE_IMAGE_DEP        *Dependencies,
  IN CONST UINTN                         DependenciesSize,
  IN CONST FMP_DEPEX_CHECK_VERSION_DATA  *FmpVersions,
  IN CONST UINTN                         FmpVersionsCount
  );

#endif