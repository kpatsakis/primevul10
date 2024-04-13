mdadm_app_hash(const void *x, size_t table_size)
{
  const mdadm_app *a = x;
  return hash_pjw(a->mdadm, table_size);
}