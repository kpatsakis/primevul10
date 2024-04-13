static MagickOffsetType GetFITSPixelExtrema(Image *image,
  const int bits_per_pixel,double *minima,double *maxima)
{
  double
    pixel;

  MagickOffsetType
    offset;

  MagickSizeType
    number_pixels;

  register MagickOffsetType
    i;

  offset=TellBlob(image);
  if (offset == -1)
    return(-1);
  number_pixels=(MagickSizeType) image->columns*image->rows;
  *minima=GetFITSPixel(image,bits_per_pixel);
  *maxima=(*minima);
  for (i=1; i < (MagickOffsetType) number_pixels; i++)
  {
    pixel=GetFITSPixel(image,bits_per_pixel);
    if (pixel < *minima)
      *minima=pixel;
    if (pixel > *maxima)
      *maxima=pixel;
  }
  return(SeekBlob(image,offset,SEEK_SET));
}