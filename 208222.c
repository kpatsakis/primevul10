main_free (void *ptr)
{
  if (ptr)
    {
      IF_DEBUG (main_mallocs -= 1);
      main_free1 (NULL, ptr);
      IF_DEBUG (XD3_ASSERT(main_mallocs >= 0));
    }
}