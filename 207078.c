attlist_copy (mu_list_t src)
{
  mu_list_t dst;

  if (!src)
    return NULL;
  dst = attlist_new ();
  mu_list_foreach (src, attlist_helper, dst);
  return dst;
}