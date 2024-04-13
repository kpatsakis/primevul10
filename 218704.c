uuid_cmp(const void *x, const void *y)
{
  const md_uuid *a = x;
  const md_uuid *b = y;

  for (size_t i = 0; i < 1; i++) {
    if (a->uuid[i] != b->uuid[i]) return 0;
  }

  return 1;
}