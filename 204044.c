request_free (struct request **req_ref)
{
  int i;
  struct request *req = *req_ref;

  if (!req)
    return;

  xfree (req->arg);
  for (i = 0; i < req->hcount; i++)
    release_header (&req->headers[i]);
  xfree (req->headers);
  xfree (req);
  *req_ref = NULL;
}