attlist_new (void)
{
  mu_list_t lst;
  int rc = mu_list_create (&lst);
  if (rc)
    {
      mu_diag_funcall (MU_DIAG_ERROR, "mu_list_create", NULL, rc);
      exit (1);
    }
  mu_list_set_destroy_item (lst, atchinfo_free);
  return lst;
}