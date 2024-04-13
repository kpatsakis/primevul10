cwd_is_root (char const *name)
{
  unsigned int prefix_len = FILE_SYSTEM_PREFIX_LEN (name);
  char root[prefix_len + 2];
  struct stat st;
  dev_t root_dev;
  ino_t root_ino;

  memcpy (root, name, prefix_len);
  root[prefix_len] = '/';
  root[prefix_len + 1] = 0;
  if (stat (root, &st))
    return false;
  root_dev = st.st_dev;
  root_ino = st.st_ino;
  if (stat (".", &st))
    return false;
  return root_dev == st.st_dev && root_ino == st.st_ino;
}