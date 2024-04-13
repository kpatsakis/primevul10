static int ipa_blob_seek(void* wand,long position)
{
  return (int)SeekBlob((Image*)wand,(MagickOffsetType) position,SEEK_SET);
}