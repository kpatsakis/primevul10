char *http_content_type(ci_request_t * req)
{
     ci_headers_list_t *heads;
     char *val;
     if (!(heads =  ci_http_response_headers(req))) {
          /* Then maybe is a reqmod request, try to get request headers */
          if (!(heads = ci_http_request_headers(req)))
               return NULL;
     }
     if (!(val = ci_headers_value(heads, "Content-Type")))
          return NULL;

     return val;
}