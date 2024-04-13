static ssize_t FindColor(PixelPacket *pixel)
{
  register ssize_t
    i;

  for (i=0; i < 256; i++)
    if (ScaleQuantumToChar(GetPixelRed(pixel)) == PalmPalette[i][0] &&
        ScaleQuantumToChar(GetPixelGreen(pixel)) == PalmPalette[i][1] &&
        ScaleQuantumToChar(GetPixelBlue(pixel)) == PalmPalette[i][2])
      return(i);
  return(-1);
}