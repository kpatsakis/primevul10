static MagickBooleanType IsFITS(const unsigned char *magick,const size_t length)
{
  if (length < 6)
    return(MagickFalse);
  if (LocaleNCompare((const char *) magick,"IT0",3) == 0)
    return(MagickTrue);
  if (LocaleNCompare((const char *) magick,"SIMPLE",6) == 0)
    return(MagickTrue);
  return(MagickFalse);
}