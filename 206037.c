bool Item::cleanup_processor(uchar *arg)
{
  if (fixed)
    cleanup();
  return FALSE;
}