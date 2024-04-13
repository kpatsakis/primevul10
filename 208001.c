int extract_http_info(ci_request_t * req, ci_headers_list_t * req_header,
                  struct http_info *httpinf)
{
     char *str;
     int i = 0;

/* Format of the HTTP header we want to parse:
	 GET http://www.squid-cache.org/Doc/config/icap_service HTTP/1.1
*/
     httpinf->url[0]='\0';
     httpinf->method[0] = '\0';

     str = req_header->headers[0];

     /* if we can't find a space character, there's somethings wrong */
     if (strchr(str, ' ') == NULL) {
          return 0;
     }

     /* extract the HTTP method */
     while (*str != ' ' && i < MAX_METHOD_SIZE) {
	httpinf->method[i] = *str;
        str++;
	i++;
     }
     httpinf->method[i] = '\0';
     ci_debug_printf(3, "DEBUG extract_http_info: method %s\n", httpinf->method);

     /* Extract the URL part of the header */
     while (*str == ' ') str++;
     i = 0;
     while (*str != ' ' && i < MAX_URL_SIZE) {
	httpinf->url[i] = *str;
	i++;
	str++;
     }
     httpinf->url[i] = '\0';
     ci_debug_printf(3, "DEBUG extract_http_info: url %s\n", httpinf->url);
     if (*str != ' ') {
          return 0;
     }
     /* we must find the HTTP version after all */
     while (*str == ' ')
          str++;
     if (*str != 'H' || *(str + 4) != '/') {
          return 0;
     }

     return 1;
}