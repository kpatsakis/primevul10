resp_free (struct response **resp_ref)
{
  struct response *resp = *resp_ref;

  if (!resp)
    return;

  xfree (resp->headers);
  xfree (resp);

  *resp_ref = NULL;
}