static inline void CopyFitsRecord(char *buffer,const char *data,
  const ssize_t offset)
{
  size_t
    length;

  if (data == (char *) NULL)
    return;
  length=MagickMin(strlen(data),80);
  if (length > (size_t) (FITSBlocksize-offset))
    length=FITSBlocksize-offset;
  (void) strncpy(buffer+offset,data,length);
}