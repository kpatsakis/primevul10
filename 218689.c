mdadm_app_free(void *x)
{
  mdadm_app *a = x;
  free(a->mdadm);
  free(a->app);
  free(a);
}