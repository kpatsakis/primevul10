connection_state_machine_loop (request_st * const r, connection * const con)
{
	request_state_t ostate;
	do {
		if (r->conf.log_state_handling) {
			log_error(r->conf.errh, __FILE__, __LINE__,
			  "state for fd:%d id:%d %s", con->fd, r->h2id,
			  connection_get_state(r->state));
		}

		switch ((ostate = r->state)) {
		case CON_STATE_REQUEST_START: /* transient */
			/*(should not be reached by HTTP/2 streams)*/
			r->start_hp.tv_sec = con->read_idle_ts = log_epoch_secs;
			if (r->conf.high_precision_timestamps)
				log_clock_gettime_realtime(&r->start_hp);

			con->request_count++;
			r->loops_per_request = 0;

			connection_set_state(r, CON_STATE_READ);
			__attribute_fallthrough__
		case CON_STATE_READ:
			/*(should not be reached by HTTP/2 streams)*/
			if (!connection_handle_read_state(con)) {
				if (r->http_version == HTTP_VERSION_2) {
					connection_transition_h2(r, con);
					connection_state_machine_h2(r, con);
					return;
				}
				break;
			}
			/*if (r->state != CON_STATE_REQUEST_END) break;*/
			__attribute_fallthrough__
		case CON_STATE_REQUEST_END: /* transient */
			ostate = (0 == r->reqbody_length)
			  ? CON_STATE_HANDLE_REQUEST
			  : CON_STATE_READ_POST;
			connection_set_state(r, ostate);
			__attribute_fallthrough__
		case CON_STATE_READ_POST:
		case CON_STATE_HANDLE_REQUEST:
			switch (http_response_handler(r)) {
			  case HANDLER_GO_ON:/*CON_STATE_RESPONSE_START occurred;transient*/
			  case HANDLER_FINISHED:
				break;
			  case HANDLER_WAIT_FOR_EVENT:
				return;
			  case HANDLER_COMEBACK:
				/* redo loop; will not match r->state */
				ostate = CON_STATE_CONNECT;
				continue;
			  case HANDLER_WAIT_FOR_FD:
		                connection_fdwaitqueue_append(con);
				return;
			  /*case HANDLER_ERROR:*/
			  default:
				connection_set_state_error(r, CON_STATE_ERROR);
				continue;
			}
			/*__attribute_fallthrough__*/
		/*case CON_STATE_RESPONSE_START:*//*occurred;transient*/
			if (r->http_version > HTTP_VERSION_1_1)
				h2_send_headers(r, con);
			else
				http_response_write_header(r);
			connection_set_state(r, CON_STATE_WRITE);
			__attribute_fallthrough__
		case CON_STATE_WRITE:
			if (connection_handle_write_state(r, con)
			    != CON_STATE_RESPONSE_END)
				break;
			__attribute_fallthrough__
		case CON_STATE_RESPONSE_END: /* transient */
		case CON_STATE_ERROR:        /* transient */
			if (r->http_version > HTTP_VERSION_1_1 && r != &con->request)
				return;
			connection_handle_response_end_state(r, con);
			break;
		case CON_STATE_CLOSE:
			/*(should not be reached by HTTP/2 streams)*/
			connection_handle_close_state(con);
			break;
		case CON_STATE_CONNECT:
			break;
		default:
			log_error(r->conf.errh, __FILE__, __LINE__,
			  "unknown state: %d %d", con->fd, r->state);
			break;
		}
	} while (ostate != (request_state_t)r->state);
}