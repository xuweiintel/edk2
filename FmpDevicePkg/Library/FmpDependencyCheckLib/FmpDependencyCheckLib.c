/** @file
  Supports Capsule Dependency Expression.

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/FmpDependencyLib.h>
#include <Library/FmpDependencyCheckLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>

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
CheckFmpDependency (
  IN CONST EFI_GUID                ImageTypeId,
  IN CONST UINT32                  Version,
  IN CONST EFI_FIRMWARE_IMAGE_DEP  *Dependencies,
  IN CONST UINT32                  DependenciesSize,
  OUT BOOLEAN                      *IsSatisfied
  )
{
  EFI_STATUS                        Status;
  EFI_HANDLE                        *HandleBuffer;
  UINTN                             Index;
  EFI_FIRMWARE_MANAGEMENT_PROTOCOL  *Fmp;
  UINTN                             ImageInfoSize;
  UINT32                            FmpImageInfoDescriptorVer;
  UINT8                             FmpImageInfoCount;
  UINTN                             DescriptorSize;
  UINT32                            PackageVersion;
  CHAR16                            *PackageVersionName;
  UINTN                             DepexSize;
  UINTN                             NumberOfFmpInstance;
  EFI_FIRMWARE_IMAGE_DESCRIPTOR     **FmpImageInfoBuf;
  FMP_DEPEX_CHECK_VERSION_DATA      *FmpVersions;
  UINTN                             FmpVersionsCount;

  if (!FeaturePcdGet (PcdFmpDependencyCheckEnable)) {
    return EFI_UNSUPPORTED;
  }

  FmpImageInfoBuf     = NULL;
  NumberOfFmpInstance = 0;
  FmpVersions         = NULL;
  FmpVersionsCount    = 0;
  *IsSatisfied        = TRUE;
  PackageVersionName  = NULL;

  //
  // Get ImageDescriptors of all FMP instances, and archive them for depex evaluation.
  //
  Status = gBS->LocateHandleBuffer (
                ByProtocol,
                &gEfiFirmwareManagementProtocolGuid,
                NULL,
                &NumberOfFmpInstance,
                &HandleBuffer
                );
  if (EFI_ERROR (Status)) {
    return EFI_ABORTED;
  }

  FmpImageInfoBuf = AllocateZeroPool (sizeof(EFI_FIRMWARE_IMAGE_DESCRIPTOR *) * NumberOfFmpInstance);
  if (FmpImageInfoBuf == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  FmpVersions = AllocateZeroPool (sizeof(FMP_DEPEX_CHECK_VERSION_DATA) * NumberOfFmpInstance);
  if (FmpVersions == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index = 0; Index < NumberOfFmpInstance; Index ++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiFirmwareManagementProtocolGuid,
                    (VOID **) &Fmp
                    );
    if (EFI_ERROR(Status)) {
      continue;
    }

    ImageInfoSize = 0;
    Status = Fmp->GetImageInfo (
                    Fmp,
                    &ImageInfoSize,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL
                    );
    if (Status != EFI_BUFFER_TOO_SMALL) {
      continue;
    }

    FmpImageInfoBuf[Index] = AllocateZeroPool (ImageInfoSize);
    if (FmpImageInfoBuf[Index] == NULL) {
      continue;
    }

    Status = Fmp->GetImageInfo (
                    Fmp,
                    &ImageInfoSize,               // ImageInfoSize
                    FmpImageInfoBuf[Index],      // ImageInfo
                    &FmpImageInfoDescriptorVer,   // DescriptorVersion
                    &FmpImageInfoCount,           // DescriptorCount
                    &DescriptorSize,              // DescriptorSize
                    &PackageVersion,              // PackageVersion
                    &PackageVersionName           // PackageVersionName
                    );
    if (EFI_ERROR(Status)) {
      FreePool (FmpImageInfoBuf[Index]);
      FmpImageInfoBuf[Index] = NULL;
      continue;
    }

    if (PackageVersionName != NULL) {
      FreePool (PackageVersionName);
      PackageVersionName = NULL;
    }

    CopyGuid(&FmpVersions[FmpVersionsCount].ImageTypeId, &FmpImageInfoBuf[Index]->ImageTypeId);
    FmpVersions[FmpVersionsCount].Version = FmpImageInfoBuf[Index]->Version;
    FmpVersionsCount ++;
  }

  //
  // Step 1 - Evaluate firmware image's depex, against the version of other Fmp instances.
  //
  if (Dependencies != NULL) {
    *IsSatisfied = EvaluateDependencies (Dependencies, DependenciesSize, FmpVersions, FmpVersionsCount);
  }

  if (!*IsSatisfied) {
    goto cleanup;
  }

  //
  // Step 2 - Evaluate the depex of all other Fmp instances, against the new version in
  // the firmware image.
  //

  //
  // Update the new version to FmpVersions.
  //
  for (Index = 0; Index < FmpVersionsCount; Index ++) {
    if (CompareGuid (&ImageTypeId, &FmpVersions[Index].ImageTypeId)) {
      FmpVersions[Index].Version = Version;
      break;
    }
  }

  //
  // Evaluate the Dependencies one by one.
  //
  for (Index = 0; Index < NumberOfFmpInstance; Index ++) {
    if (FmpImageInfoBuf[Index] != NULL) {
      //
      // Skip the Fmp instance to be "SetImage".
      //
      if (CompareGuid (&ImageTypeId, &FmpImageInfoBuf[Index]->ImageTypeId)) {
        continue;
      }
      if ((FmpImageInfoBuf[Index]->AttributesSetting & IMAGE_ATTRIBUTE_DEPENDENCY) &&
          (FmpImageInfoBuf[Index]->Dependencies != NULL)) {
        //
        // Get the size of depex.
        // Assume that the dependencies in EFI_FIRMWARE_IMAGE_DESCRIPTOR is validated when PopulateDescriptor().
        //
        DepexSize = GetDependencySize (FmpImageInfoBuf[Index]->Dependencies);
        if (DepexSize > 0) {
          *IsSatisfied = EvaluateDependencies (FmpImageInfoBuf[Index]->Dependencies, DepexSize, FmpVersions, FmpVersionsCount);
          if (!*IsSatisfied) {
            break;
          }
        }
      }
    }
  }

cleanup:
  if (FmpImageInfoBuf != NULL) {
    for (Index = 0; Index < NumberOfFmpInstance; Index ++) {
      if (FmpImageInfoBuf[Index] != NULL) {
        FreePool (FmpImageInfoBuf[Index]);
      }
    }
    FreePool (FmpImageInfoBuf);
  }

  if (FmpVersions != NULL) {
    FreePool (FmpVersions);
  }

  return EFI_SUCCESS;
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
