int squidclamav_write_to_net(char *buf, int len, ci_request_t * req)
{
     int bytes;
     av_req_data_t *data = ci_service_data(req);

     if (!data)
          return CI_ERROR;

     if (data->blocked == 1 && data->error_page == 0) {
	ci_debug_printf(2, "DEBUG squidclamav_write_to_net: ending here, content was blocked\n");
	return CI_EOF; 
     }
     if (data->virus == 1 && data->error_page == 0) {
	ci_debug_printf(2, "DEBUG squidclamav_write_to_net: ending here, virus was found\n");
	return CI_EOF; 
     }

     /* if a virus was found or the page has been blocked, a warning page
	has already been generated */
     if (data->error_page)
          return ci_membuf_read(data->error_page, buf, len);

     if (data->body)
	bytes = ci_simple_file_read(data->body, buf, len);
     else
	 bytes =0;

     return bytes;
}