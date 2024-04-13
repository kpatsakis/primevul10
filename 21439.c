is_dir (char const *filename, int flags, glob_t const *pglob)
{
  struct stat st;
  struct_stat64 st64;
  return (__glibc_unlikely (flags & GLOB_ALTDIRFUNC)
          ? pglob->gl_stat (filename, &st) == 0 && S_ISDIR (st.st_mode)
          : __stat64 (filename, &st64) == 0 && S_ISDIR (st64.st_mode));
}