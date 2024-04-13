connection_state_machine_h2 (request_st * const h2r, connection * const con)
{
    h2con * const h2c = con->h2;

    if (h2c->sent_goaway <= 0
        && (chunkqueue_is_empty(con->read_queue) || h2_parse_frames(con))
        && con->is_readable > 0) {
        chunkqueue * const cq = con->read_queue;
        const off_t mark = cq->bytes_in;
        if (0 == con->network_read(con, cq, MAX_READ_LIMIT)) {
            if (mark < cq->bytes_in)
                h2_parse_frames(con);
        }
        else {
            /* network error; do not send GOAWAY, but pretend that we did */
            h2c->sent_goaway = H2_E_CONNECT_ERROR; /*any error (not NO_ERROR)*/
            connection_set_state_error(h2r, CON_STATE_ERROR);
        }
    }

    /* process requests on HTTP/2 streams */
    int resched = 0;
    if (h2c->sent_goaway <= 0 && h2c->rused) {
        /* coarse check for write throttling
         * (connection.kbytes-per-second, server.kbytes-per-second)
         * obtain an approximate limit, not refreshed per request_st,
         * even though we are not calculating response HEADERS frames
         * or frame overhead here */
        off_t max_bytes = con->is_writable > 0
          ? connection_write_throttle(con, MAX_WRITE_LIMIT)
          : 0;
        const off_t fsize = (off_t)h2c->s_max_frame_size;

        /* XXX: to avoid buffer bloat due to staging too much data in
         * con->write_queue, consider setting limit on how much is staged
         * for sending on con->write_queue: adjusting max_bytes down */

        /* XXX: TODO: process requests in stream priority order */
        for (uint32_t i = 0; i < h2c->rused; ++i) {
            request_st * const r = h2c->r[i];
            /* future: might track read/write interest per request
             * to avoid iterating through all active requests */

          #if 0
            const int log_state_handling = r->conf.log_state_handling;
            if (log_state_handling)
                log_error(r->conf.errh, __FILE__, __LINE__,
                  "state at enter %d %d %s", con->fd, r->h2id,
                  connection_get_state(r->state));
          #endif

            connection_state_machine_loop(r, con);

            if (r->resp_header_len && !chunkqueue_is_empty(&r->write_queue)
                && (r->resp_body_finished
                    || (r->conf.stream_response_body
                        & (FDEVENT_STREAM_RESPONSE
                          |FDEVENT_STREAM_RESPONSE_BUFMIN)))) {

                chunkqueue * const cq = &r->write_queue;
                off_t avail = chunkqueue_length(cq);
                if (avail > max_bytes)    avail = max_bytes;
                if (avail > fsize)        avail = fsize;
                if (avail > r->h2_swin)   avail = r->h2_swin;
                if (avail > h2r->h2_swin) avail = h2r->h2_swin;

                if (avail > 0) {
                    max_bytes -= avail;
                    h2_send_cqdata(r, con, cq, (uint32_t)avail);
                }

                if (r->resp_body_finished && chunkqueue_is_empty(cq)) {
                    connection_set_state(r, CON_STATE_RESPONSE_END);
                    if (r->conf.log_state_handling)
                        connection_state_machine_loop(r, con);
                }
                else if (avail) /*(do not spin if swin empty window)*/
                    resched |= (!chunkqueue_is_empty(cq));
            }

          #if 0
            if (log_state_handling)
                log_error(r->conf.errh, __FILE__, __LINE__,
                  "state at exit %d %d %s", con->fd, r->h2id,
                  connection_get_state(r->state));
          #endif

            if (r->state==CON_STATE_RESPONSE_END || r->state==CON_STATE_ERROR) {
                /*(trigger reschedule of con if frames pending)*/
                if (h2c->rused == sizeof(h2c->r)/sizeof(*h2c->r)
                    && !chunkqueue_is_empty(con->read_queue))
                    resched |= 1;
                h2_send_end_stream(r, con);
                h2_retire_stream(r, con);/*r invalidated;removed from h2c->r[]*/
                --i;/* adjust loop i; h2c->rused was modified to retire r */
            }
        }
    }

    if (h2c->sent_goaway > 0 && h2c->rused) {
        /* retire streams if an error has occurred
         * note: this is not done to other streams in the loop above
         * (besides the current stream in the loop) due to the specific
         * implementation above, where doing so would mess up the iterator */
        for (uint32_t i = 0; i < h2c->rused; ++i) {
            request_st * const r = h2c->r[i];
            /*assert(r->h2state == H2_STATE_CLOSED);*/
            h2_retire_stream(r, con);/*r invalidated;removed from h2c->r[]*/
            --i;/* adjust loop i; h2c->rused was modified to retire r */
        }
        /* XXX: ? should we discard con->write_queue
         *        and change h2r->state to CON_STATE_RESPONSE_END ? */
    }

    if (h2r->state == CON_STATE_WRITE) {
        /* write HTTP/2 frames to socket */
        if (!chunkqueue_is_empty(con->write_queue))
            connection_handle_write(h2r, con);

        if (chunkqueue_is_empty(con->write_queue)
            && 0 == h2c->rused && h2c->sent_goaway)
            connection_set_state(h2r, CON_STATE_RESPONSE_END);
    }

    if (h2r->state == CON_STATE_WRITE) {
        if (resched && !con->traffic_limit_reached)
            joblist_append(con);

        if (h2_want_read(con))
            h2r->conf.stream_request_body |=  FDEVENT_STREAM_REQUEST_POLLIN;
        else
            h2r->conf.stream_request_body &= ~FDEVENT_STREAM_REQUEST_POLLIN;
    }
    else /* e.g. CON_STATE_RESPONSE_END or CON_STATE_ERROR */
        connection_state_machine_loop(h2r, con);

    connection_set_fdevent_interest(h2r, con);
}