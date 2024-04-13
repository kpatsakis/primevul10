url_uses_proxy (struct url * u)
{
  bool ret;
  char *proxy;

  if (!u)
    return false;
  proxy = getproxy (u);
  ret = proxy != NULL;
  xfree (proxy);
  return ret;
}