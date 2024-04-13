static uint ms_to_precision(uint ms)
{
  uint cut, precision;
  for (cut= 10, precision= 6 ; precision > 0 ; cut*= 10, precision--)
  {
    if (ms % cut)
      return precision;
  }
  return 0;
}