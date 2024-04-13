glob_use_alloca (size_t alloca_used, size_t len)
{
  size_t size;
  return (!size_add_wrapv (alloca_used, len, &size)
          && __libc_use_alloca (size));
}