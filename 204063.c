request_new (const char *method, char *arg)
{
  struct request *req = xnew0 (struct request);
  req->hcapacity = 8;
  req->headers = xnew_array (struct request_header, req->hcapacity);
  req->method = method;
  req->arg = arg;
  return req;
}