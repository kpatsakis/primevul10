MagickExport MagickBooleanType GetImageEntropy(const Image *image,
  double *entropy,ExceptionInfo *exception)
{
  MagickBooleanType
    status;

  status=GetImageChannelEntropy(image,CompositeChannels,entropy,exception);
  return(status);
}