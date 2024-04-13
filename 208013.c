void squidclamav_release_request_data(void *data)
{

     if (data) {
          ci_debug_printf(1, "DEBUG squidclamav_release_request_data: Releasing request data.\n");

        if (((av_req_data_t *) data)->body) {
           ci_simple_file_destroy(((av_req_data_t *) data)->body);
	   if (((av_req_data_t *) data)->url)
		ci_buffer_free(((av_req_data_t *) data)->url);
	   if (((av_req_data_t *) data)->user)
		ci_buffer_free(((av_req_data_t *) data)->user);
	   if (((av_req_data_t *) data)->clientip)
		ci_buffer_free(((av_req_data_t *) data)->clientip);
	}

        if (((av_req_data_t *) data)->error_page)
           ci_membuf_free(((av_req_data_t *) data)->error_page);

        ci_object_pool_free(data);
     }
}