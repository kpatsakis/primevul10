void generate_redirect_page(char * redirect, ci_request_t * req, av_req_data_t * data)
{
     int new_size = 0;
     char buf[MAX_URL_SIZE];
     ci_membuf_t *error_page;

     new_size = strlen(blocked_header_message) + strlen(redirect) + strlen(blocked_footer_message) + 10;

     if ( ci_http_response_headers(req))
          ci_http_response_reset_headers(req);
     else
          ci_http_response_create(req, 1, 1);

     ci_debug_printf(2, "DEBUG generate_redirect_page: creating redirection page\n");

     snprintf(buf, MAX_URL_SIZE, "Location: %s", redirect);
     /*strcat(buf, ";");*/

     ci_debug_printf(3, "DEBUG generate_redirect_page: %s\n", buf);

     ci_http_response_add_header(req, "HTTP/1.0 301 Moved Permanently");
     ci_http_response_add_header(req, buf);
     ci_http_response_add_header(req, "Server: C-ICAP");
     ci_http_response_add_header(req, "Connection: close");
     /*ci_http_response_add_header(req, "Content-Type: text/html;");*/
     ci_http_response_add_header(req, "Content-Type: text/html");
     ci_http_response_add_header(req, "Content-Language: en");

     if (data->blocked == 1) {
	error_page = ci_membuf_new_sized(new_size);
	((av_req_data_t *) data)->error_page = error_page;
	ci_membuf_write(error_page, (char *) blocked_header_message, strlen(blocked_header_message), 0);
	ci_membuf_write(error_page, (char *) redirect, strlen(redirect), 0);
	ci_membuf_write(error_page, (char *) blocked_footer_message, strlen(blocked_footer_message), 1);
     }
     ci_debug_printf(3, "DEBUG generate_redirect_page: done\n");

}