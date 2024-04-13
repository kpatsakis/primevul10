connection_set_fdevent_interest (request_st * const r, connection * const con)
{
    if (con->fd < 0) return;

    if (con->revents_err && r->state != CON_STATE_ERROR) {
        connection_revents_err(r, con); /* resets con->revents_err = 0 */
        connection_state_machine(con);
        return;
        /* connection_state_machine() will end up calling back into
         * connection_set_fdevent_interest(), but with 0 == con->revents_err */
    }

    int n = 0;
    switch(r->state) {
      case CON_STATE_READ:
        n = FDEVENT_IN;
        if (!(r->conf.stream_request_body & FDEVENT_STREAM_REQUEST_POLLRDHUP))
            n |= FDEVENT_RDHUP;
        break;
      case CON_STATE_WRITE:
        if (!chunkqueue_is_empty(con->write_queue)
            && 0 == con->is_writable && 0 == con->traffic_limit_reached)
            n |= FDEVENT_OUT;
        __attribute_fallthrough__
      case CON_STATE_READ_POST:
        if (r->conf.stream_request_body & FDEVENT_STREAM_REQUEST_POLLIN)
            n |= FDEVENT_IN;
        if (!(r->conf.stream_request_body & FDEVENT_STREAM_REQUEST_POLLRDHUP))
            n |= FDEVENT_RDHUP;
        break;
      case CON_STATE_CLOSE:
        n = FDEVENT_IN;
        break;
      case CON_STATE_CONNECT:
        return;
      default:
        break;
    }

    const int events = fdevent_fdnode_interest(con->fdn);
    if (con->is_readable < 0) {
        con->is_readable = 0;
        n |= FDEVENT_IN;
    }
    if (con->is_writable < 0) {
        con->is_writable = 0;
        n |= FDEVENT_OUT;
    }
    if (events & FDEVENT_RDHUP)
        n |= FDEVENT_RDHUP;

    if (n == events) return;

    /* update timestamps when enabling interest in events */
    if ((n & FDEVENT_IN) && !(events & FDEVENT_IN))
        con->read_idle_ts = log_epoch_secs;
    if ((n & FDEVENT_OUT) && !(events & FDEVENT_OUT))
        con->write_request_ts = log_epoch_secs;
    fdevent_fdnode_event_set(con->srv->ev, con->fdn, n);
}