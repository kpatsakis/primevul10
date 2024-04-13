MagickExport MagickBooleanType GetImageExtrema(const Image *image,
  size_t *minima,size_t *maxima,ExceptionInfo *exception)
{
  MagickBooleanType
    status;

  status=GetImageChannelExtrema(image,CompositeChannels,minima,maxima,
    exception);
  return(status);
}