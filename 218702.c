md_uuid_free(void *x)
{
  md_uuid *a = x;
  free(a->path);
  free(a);
}