rpl_realloc (void *p, size_t n)
{
  void *result;

#if NEED_REALLOC_GNU
  if (n == 0)
    {
      n = 1;

      /* In theory realloc might fail, so don't rely on it to free.  */
      free (p);
      p = NULL;
    }
#endif

  if (p == NULL)
    {
#if GNULIB_REALLOC_GNU && !NEED_REALLOC_GNU && !SYSTEM_MALLOC_GLIBC_COMPATIBLE
      if (n == 0)
        n = 1;
#endif
      result = malloc (n);
    }
  else
    result = realloc (p, n);

#if !HAVE_REALLOC_POSIX
  if (result == NULL)
    errno = ENOMEM;
#endif

  return result;
}