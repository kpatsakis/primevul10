int magick_progress_callback(void *context,float quantum)
{
  Image
    *image;

  MagickBooleanType
    status;

  (void) quantum;
  image=(Image *) context;
  assert(image->signature == MagickCoreSignature);
  status=SetImageProgress(image,LoadImagesTag,TellBlob(image),
    GetBlobSize(image));
  return(status != MagickFalse ? 0 : 1);
}