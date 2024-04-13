int squidclamav_read_from_net(char *buf, int len, int iseof, ci_request_t * req)
{
     av_req_data_t *data = ci_service_data(req);
     int allow_transfer;

     if (!data)
          return CI_ERROR;

     if (!data->body)
	return len;

    if (data->no_more_scan == 1) {
	return ci_simple_file_write(data->body, buf, len, iseof);
    }

    if ((maxsize > 0) && (data->body->bytes_in >= maxsize)) {
	data->no_more_scan = 1;
	ci_req_unlock_data(req);
	ci_simple_file_unlock_all(data->body);
	ci_debug_printf(1, "DEBUG squidclamav_read_from_net: No more antivir check, downloaded stream is upper than maxsize (%d>%d)\n", data->body->bytes_in, (int)maxsize);
    } else if (SEND_PERCENT_BYTES && (START_SEND_AFTER < data->body->bytes_in)) {
	ci_req_unlock_data(req);
	allow_transfer = (SEND_PERCENT_BYTES * (data->body->endpos + len)) / 100;
	ci_simple_file_unlock(data->body, allow_transfer);
    }

    return ci_simple_file_write(data->body, buf, len, iseof);
}