void *squidclamav_init_request_data(ci_request_t * req)
{
    int preview_size;
    av_req_data_t *data;

    preview_size = ci_req_preview_size(req);

    ci_debug_printf(1, "DEBUG squidclamav_init_request_data: initializing request data handler.\n");

    if (!(data = ci_object_pool_alloc(AVREQDATA_POOL))) {
	ci_debug_printf(0, "FATAL squidclamav_init_request_data: Error allocation memory for service data!!!");
	return NULL;
    }
    data->body = NULL;
    data->error_page = NULL;
    data->req = req;
    data->blocked = 0;
    data->no_more_scan = 0;
    data->virus = 0;

    return data;
}