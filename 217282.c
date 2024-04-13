MagickExport Image *PolynomialImage(const Image *images,
  const size_t number_terms,const double *terms,ExceptionInfo *exception)
{
  Image
    *polynomial_image;

  polynomial_image=PolynomialImageChannel(images,DefaultChannels,number_terms,
    terms,exception);
  return(polynomial_image);
}