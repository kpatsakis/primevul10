MagickExport MagickBooleanType FunctionImage(Image *image,
  const MagickFunction function,const size_t number_parameters,
  const double *parameters,ExceptionInfo *exception)
{
  MagickBooleanType
    status;

  status=FunctionImageChannel(image,CompositeChannels,function,
    number_parameters,parameters,exception);
  return(status);
}