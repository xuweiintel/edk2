#include <Library/FmpDependencyLib.h>

EFI_STATUS
EvaluateDepexInCap (
  IN CONST EFI_FIRMWARE_IMAGE_DEP  *Depex,
  IN CONST UINT32                  DepexSize,
  OUT BOOLEAN                      *IsSatisfied
);


EFI_STATUS
EvaluateDepexFromDevice (
  IN CONST EFI_GUID                ImageTypeId,
  IN CONST UINT32                  Version,
  OUT BOOLEAN                      *IsSatisfied
);




EFI_STATUS
EvaluateDependencies (
  IN CONST EFI_GUID                ImageTypeId,
  IN CONST UINT32                  Version,
  IN CONST EFI_FIRMWARE_IMAGE_DEP  *Depex,
  IN CONST UINT32                  DepexSize,
  OUT BOOLEAN                      *IsSatisfied
  )
{
  //
  // EvaluateDepexInCap in mandatory.
  //
  EFI_STATUS Status;

  Status = EvaluateDepexInCap(Depex, DepexSize, IsSatisfied);

  if (EFI_ERROR(Status) || *IsSatisfied == FALSE) {
    return Status;
  }

  //
  // EvaluateDepexFromDevice is optional
  //
  if (PcdGetBool(PcdEvaluateDepexFromDevcie)) {
    Status = EvaluateDepexFromDevice(ImageTypeId, Version, IsSatisfied);
  }

  return Status;
}


77056