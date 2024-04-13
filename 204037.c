http_cleanup (void)
{
  xfree (pconn.host);
  if (wget_cookie_jar)
    cookie_jar_delete (wget_cookie_jar);
}