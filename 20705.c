static inline double GetFITSPixel(Image *image,int bits_per_pixel)
{
  switch (image->depth >> 3)
  {
    case 1:
      return((double) ReadBlobByte(image));
    case 2:
      return((double) ((short) ReadBlobShort(image)));
    case 4:
    {
      if (bits_per_pixel > 0)
        return((double) ReadBlobSignedLong(image));
      return((double) ReadBlobFloat(image));
    }
    case 8:
    {
      if (bits_per_pixel > 0)
        return((double) ((MagickOffsetType) ReadBlobLongLong(image)));
    }
    default:
      break;
  }
  return(ReadBlobDouble(image));
}