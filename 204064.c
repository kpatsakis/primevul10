check_retry_on_http_error (const int statcode)
{
  const char *tok = opt.retry_on_http_error;
  while (tok && *tok)
    {
      if (atoi (tok) == statcode)
        return true;
      if ((tok = strchr (tok, ',')))
        ++tok;
    }
  return false;
}