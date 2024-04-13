static int IntensityCompare(const void *x,const void *y)
{
  const MagickPixelPacket
    *color_1,
    *color_2;

  int
    intensity;

  color_1=(const MagickPixelPacket *) x;
  color_2=(const MagickPixelPacket *) y;
  intensity=(int) MagickPixelIntensity(color_2)-(int)
    MagickPixelIntensity(color_1);
  return(intensity);
}