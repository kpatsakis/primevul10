fill_body (mu_message_t msg, mu_stream_t instr)
{
  int rc;
  mu_body_t body = NULL;
  mu_stream_t stream = NULL;
  mu_off_t n;
  
  rc = mu_message_get_body (msg, &body);
  if (rc)
    {
      mu_error (_("cannot get message body: %s"), mu_strerror (rc));
      return 1;
    }
  rc = mu_body_get_streamref (body, &stream);
  if (rc)
    {
      mu_error (_("cannot get body: %s"), mu_strerror (rc));
      return 1;
    }

  rc = mu_stream_copy (stream, instr, 0, &n);
  mu_stream_destroy (&stream);

  if (rc)
    {
      mu_error (_("cannot copy temporary stream: %s"), mu_strerror (rc));
      return 1;
    }
  
  if (n == 0)
    {
      if (mailvar_is_true (mailvar_name_nullbody))
	{
	  char *str;
	  if (mailvar_get (&str, mailvar_name_nullbodymsg,
			   mailvar_type_string, 0) == 0)
	    mu_error ("%s", _(str));
	}
      else
	return 1;
    }

  return 0;
}