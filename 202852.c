file_id_hasher (void const *entry, size_t table_size)
{
  file_id const *e = entry;
  size_t i = e->ino + e->dev;
  return i % table_size;
}