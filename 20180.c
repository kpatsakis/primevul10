static long ipa_blob_tell(void* wand)
{
  return (long)TellBlob((Image*)wand);
}