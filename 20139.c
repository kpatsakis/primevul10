static int WMFSeekBlob(void *image,long offset)
{
  return((int) SeekBlob((Image *) image,(MagickOffsetType) offset,SEEK_SET));
}