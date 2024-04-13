connection_revents_err (request_st * const r, connection * const con)
{
    /* defer handling FDEVENT_HUP and FDEVENT_ERR to here in order to
     * first attempt (in callers) to read data in kernel socket buffers */
    /*assert(con->revents_err & ~(FDEVENT_IN | FDEVENT_OUT));*/
    const int revents = (int)con->revents_err;
    con->revents_err = 0;

    if (r->state == CON_STATE_CLOSE)
        con->close_timeout_ts = log_epoch_secs - (HTTP_LINGER_TIMEOUT+1);
    else if (revents & FDEVENT_HUP)
        connection_set_state_error(r, CON_STATE_ERROR);
    else if (revents & FDEVENT_RDHUP) {
        int events = fdevent_fdnode_interest(con->fdn);
        events &= ~(FDEVENT_IN|FDEVENT_RDHUP);
        r->conf.stream_request_body &=
          ~(FDEVENT_STREAM_REQUEST_BUFMIN|FDEVENT_STREAM_REQUEST_POLLIN);
        r->conf.stream_request_body |= FDEVENT_STREAM_REQUEST_POLLRDHUP;
        con->is_readable = 1; /*(can read 0 for end-of-stream)*/
        if (chunkqueue_is_empty(con->read_queue)) r->keep_alive = 0;
        if (r->reqbody_length < -1)/*(transparent proxy mode; no more rd data)*/
            r->reqbody_length = r->reqbody_queue.bytes_in;
        if (sock_addr_get_family(&con->dst_addr) == AF_UNIX) {
            /* future: will getpeername() on AF_UNIX check if still connected?*/
            fdevent_fdnode_event_set(con->srv->ev, con->fdn, events);
        }
        else if (fdevent_is_tcp_half_closed(con->fd)) {
            /* Success of fdevent_is_tcp_half_closed() after FDEVENT_RDHUP
             * indicates TCP FIN received, but does not distinguish between
             * client shutdown(fd, SHUT_WR) and client close(fd).  Remove
             * FDEVENT_RDHUP so that we do not spin on ready event.  However,
             * a later TCP RST will not be detected until next write to socket.
             * future: might getpeername() to check for TCP RST on half-closed
             * sockets (without FDEVENT_RDHUP interest) when checking for write
             * timeouts once a second in server.c, though getpeername() on
             * Windows might not indicate this */
            r->conf.stream_request_body |= FDEVENT_STREAM_REQUEST_TCP_FIN;
            fdevent_fdnode_event_set(con->srv->ev, con->fdn, events);
        }
        else {
            /* Failure of fdevent_is_tcp_half_closed() indicates TCP RST
             * (or unable to tell (unsupported OS), though should not
             * be setting FDEVENT_RDHUP in that case) */
            connection_set_state_error(r, CON_STATE_ERROR);
        }
    }
    else if (revents & FDEVENT_ERR)  /* error, connection reset */
        connection_set_state_error(r, CON_STATE_ERROR);
    else
        log_error(r->conf.errh, __FILE__, __LINE__,
          "connection closed: poll() -> ??? %d", revents);
}