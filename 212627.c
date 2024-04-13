packet_queue_listener(LIBSSH2_SESSION * session, unsigned char *data,
                      unsigned long datalen,
                      packet_queue_listener_state_t *listen_state)
{
    /*
     * Look for a matching listener
     */
    /* 17 = packet_type(1) + channel(4) + reason(4) + descr(4) + lang(4) */
    unsigned long packet_len = 17 + (sizeof(FwdNotReq) - 1);
    unsigned char *p;
    LIBSSH2_LISTENER *listn = _libssh2_list_first(&session->listeners);
    char failure_code = SSH_OPEN_ADMINISTRATIVELY_PROHIBITED;
    int rc;

    (void) datalen;

    if(listen_state->state == libssh2_NB_state_idle) {
        unsigned char *s = data + (sizeof("forwarded-tcpip") - 1) + 5;
        listen_state->sender_channel = _libssh2_ntohu32(s);
        s += 4;

        listen_state->initial_window_size = _libssh2_ntohu32(s);
        s += 4;
        listen_state->packet_size = _libssh2_ntohu32(s);
        s += 4;

        listen_state->host_len = _libssh2_ntohu32(s);
        s += 4;
        listen_state->host = s;
        s += listen_state->host_len;
        listen_state->port = _libssh2_ntohu32(s);
        s += 4;

        listen_state->shost_len = _libssh2_ntohu32(s);
        s += 4;
        listen_state->shost = s;
        s += listen_state->shost_len;
        listen_state->sport = _libssh2_ntohu32(s);

        _libssh2_debug(session, LIBSSH2_TRACE_CONN,
                       "Remote received connection from %s:%ld to %s:%ld",
                       listen_state->shost, listen_state->sport,
                       listen_state->host, listen_state->port);

        listen_state->state = libssh2_NB_state_allocated;
    }

    if(listen_state->state != libssh2_NB_state_sent) {
        while(listn) {
            if((listn->port == (int) listen_state->port) &&
                (strlen(listn->host) == listen_state->host_len) &&
                (memcmp (listn->host, listen_state->host,
                         listen_state->host_len) == 0)) {
                /* This is our listener */
                LIBSSH2_CHANNEL *channel = NULL;
                listen_state->channel = NULL;

                if(listen_state->state == libssh2_NB_state_allocated) {
                    if(listn->queue_maxsize &&
                        (listn->queue_maxsize <= listn->queue_size)) {
                        /* Queue is full */
                        failure_code = SSH_OPEN_RESOURCE_SHORTAGE;
                        _libssh2_debug(session, LIBSSH2_TRACE_CONN,
                                       "Listener queue full, ignoring");
                        listen_state->state = libssh2_NB_state_sent;
                        break;
                    }

                    channel = LIBSSH2_CALLOC(session, sizeof(LIBSSH2_CHANNEL));
                    if(!channel) {
                        _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                                       "Unable to allocate a channel for "
                                       "new connection");
                        failure_code = SSH_OPEN_RESOURCE_SHORTAGE;
                        listen_state->state = libssh2_NB_state_sent;
                        break;
                    }
                    listen_state->channel = channel;

                    channel->session = session;
                    channel->channel_type_len = sizeof("forwarded-tcpip") - 1;
                    channel->channel_type = LIBSSH2_ALLOC(session,
                                                          channel->
                                                          channel_type_len +
                                                          1);
                    if(!channel->channel_type) {
                        _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                                       "Unable to allocate a channel for new"
                                       " connection");
                        LIBSSH2_FREE(session, channel);
                        failure_code = SSH_OPEN_RESOURCE_SHORTAGE;
                        listen_state->state = libssh2_NB_state_sent;
                        break;
                    }
                    memcpy(channel->channel_type, "forwarded-tcpip",
                           channel->channel_type_len + 1);

                    channel->remote.id = listen_state->sender_channel;
                    channel->remote.window_size_initial =
                        LIBSSH2_CHANNEL_WINDOW_DEFAULT;
                    channel->remote.window_size =
                        LIBSSH2_CHANNEL_WINDOW_DEFAULT;
                    channel->remote.packet_size =
                        LIBSSH2_CHANNEL_PACKET_DEFAULT;

                    channel->local.id = _libssh2_channel_nextid(session);
                    channel->local.window_size_initial =
                        listen_state->initial_window_size;
                    channel->local.window_size =
                        listen_state->initial_window_size;
                    channel->local.packet_size = listen_state->packet_size;

                    _libssh2_debug(session, LIBSSH2_TRACE_CONN,
                                   "Connection queued: channel %lu/%lu "
                                   "win %lu/%lu packet %lu/%lu",
                                   channel->local.id, channel->remote.id,
                                   channel->local.window_size,
                                   channel->remote.window_size,
                                   channel->local.packet_size,
                                   channel->remote.packet_size);

                    p = listen_state->packet;
                    *(p++) = SSH_MSG_CHANNEL_OPEN_CONFIRMATION;
                    _libssh2_store_u32(&p, channel->remote.id);
                    _libssh2_store_u32(&p, channel->local.id);
                    _libssh2_store_u32(&p,
                                       channel->remote.window_size_initial);
                    _libssh2_store_u32(&p, channel->remote.packet_size);

                    listen_state->state = libssh2_NB_state_created;
                }

                if(listen_state->state == libssh2_NB_state_created) {
                    rc = _libssh2_transport_send(session, listen_state->packet,
                                                 17, NULL, 0);
                    if(rc == LIBSSH2_ERROR_EAGAIN)
                        return rc;
                    else if(rc) {
                        listen_state->state = libssh2_NB_state_idle;
                        return _libssh2_error(session, rc,
                                              "Unable to send channel "
                                              "open confirmation");
                    }

                    /* Link the channel into the end of the queue list */
                    if(listen_state->channel) {
                        _libssh2_list_add(&listn->queue,
                                          &listen_state->channel->node);
                        listn->queue_size++;
                    }

                    listen_state->state = libssh2_NB_state_idle;
                    return 0;
                }
            }

            listn = _libssh2_list_next(&listn->node);
        }

        listen_state->state = libssh2_NB_state_sent;
    }

    /* We're not listening to you */
    p = listen_state->packet;
    *(p++) = SSH_MSG_CHANNEL_OPEN_FAILURE;
    _libssh2_store_u32(&p, listen_state->sender_channel);
    _libssh2_store_u32(&p, failure_code);
    _libssh2_store_str(&p, FwdNotReq, sizeof(FwdNotReq) - 1);
    _libssh2_htonu32(p, 0);

    rc = _libssh2_transport_send(session, listen_state->packet,
                                 packet_len, NULL, 0);
    if(rc == LIBSSH2_ERROR_EAGAIN) {
        return rc;
    }
    else if(rc) {
        listen_state->state = libssh2_NB_state_idle;
        return _libssh2_error(session, rc, "Unable to send open failure");

    }
    listen_state->state = libssh2_NB_state_idle;
    return 0;
}