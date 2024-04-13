connection_write_1xx_info (request_st * const r, connection * const con)
{
    /* (Note: prior 1xx intermediate responses may be present in cq) */
    /* (Note: also choosing not to update con->write_request_ts
     *  which differs from connection_write_chunkqueue()) */
    chunkqueue * const cq = con->write_queue;
    off_t written = cq->bytes_out;

    int rc = con->network_write(con, cq, MAX_WRITE_LIMIT);

    written = cq->bytes_out - written;
    con->bytes_written += written;
    con->bytes_written_cur_second += written;
    if (r->conf.global_bytes_per_second_cnt_ptr)
        *(r->conf.global_bytes_per_second_cnt_ptr) += written;

    if (rc < 0) {
        connection_set_state_error(r, CON_STATE_ERROR);
        return 0; /* error */
    }

    if (!chunkqueue_is_empty(cq)) { /* partial write (unlikely) */
        con->is_writable = 0;
        if (cq == &r->write_queue) {
            /* save partial write of 1xx in separate chunkqueue
             * Note: sending of remainder of 1xx might be delayed
             * until next set of response headers are sent */
            con->write_queue = chunkqueue_init(NULL);
            chunkqueue_append_chunkqueue(con->write_queue, cq);
        }
    }

  #if 0
    /* XXX: accounting inconsistency
     * 1xx is not currently included in r->resp_header_len,
     * so mod_accesslog reporting of %b or %B (FORMAT_BYTES_OUT_NO_HEADER)
     * reports all bytes out minus len of final response headers,
     * but including 1xx intermediate responses.  If 1xx intermediate
     * responses were included in r->resp_header_len, then there are a
     * few places in the code which must be adjusted to use r->resp_header_done
     * instead of (0 == r->resp_header_len) as flag that final response was set
     * (Doing the following would "discard" the 1xx len from bytes_out)
     */
    r->write_queue.bytes_in = r->write_queue.bytes_out = 0;
  #endif

    return 1; /* success */
}