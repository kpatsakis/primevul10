static void *DestroyGIFProfile(void *profile)
{
  return((void *) DestroyStringInfo((StringInfo *) profile));
}