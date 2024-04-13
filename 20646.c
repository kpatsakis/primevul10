static void SetFITSUnsignedPixels(const size_t length,
  const size_t bits_per_pixel,const EndianType endian,unsigned char *pixels)
{
  register ssize_t
    i;

  if (endian != MSBEndian)
    pixels+=(bits_per_pixel >> 3)-1;
  for (i=0; i < (ssize_t) length; i++)
  {
    *pixels^=0x80;
    pixels+=bits_per_pixel >> 3;
  }
}