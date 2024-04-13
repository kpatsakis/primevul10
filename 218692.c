uuid_hash(const void *x, size_t table_size)
{
  const md_uuid *a = x;

  size_t h = a->uuid[0];
  for (size_t i = 1; i < 4; i++) {
    h ^= a->uuid[i];
  }

  return h % table_size;
}