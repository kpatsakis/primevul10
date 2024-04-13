mdadm_app_cmp(const void *x, const void *y)
{
  const mdadm_app *a = x;
  const mdadm_app *b = y;

  return STREQ (a->mdadm, b->mdadm);
}