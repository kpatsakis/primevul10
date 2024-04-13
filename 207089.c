message_add_date (mu_message_t msg)
{
  mu_header_t hdr;
  char buf[MU_DATETIME_RFC822_LENGTH+1];
  struct tm ltm;
  time_t t;
  int rc;
  
  rc = mu_message_get_header (msg, &hdr);
  if (rc)
    {
      mu_diag_funcall (MU_DIAG_ERROR, "mu_message_get_header", NULL, rc);
      return;
    }

  t = time (NULL);
  localtime_r (&t, &ltm);
  
  mu_strftime (buf, sizeof (buf), MU_DATETIME_FORM_RFC822, &ltm);
  rc = mu_header_set_value (hdr, MU_HEADER_DATE, buf, 1);
  if (rc)
    mu_diag_funcall (MU_DIAG_ERROR, "mu_header_set_value", MU_HEADER_DATE, rc);
}