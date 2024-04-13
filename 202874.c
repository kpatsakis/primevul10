file_already_seen (struct stat const *st)
{
  file_id f;
  f.dev = st->st_dev;
  f.ino = st->st_ino;
  return hash_lookup (file_id_table, &f) != 0;
}