packet_x11_open(LIBSSH2_SESSION * session, unsigned char *data,
                unsigned long datalen,
                packet_x11_open_state_t *x11open_state)
{
    int failure_code = SSH_OPEN_CONNECT_FAILED;
    /* 17 = packet_type(1) + channel(4) + reason(4) + descr(4) + lang(4) */
    unsigned long packet_len = 17 + (sizeof(X11FwdUnAvil) - 1);
    unsigned char *p;
    LIBSSH2_CHANNEL *channel = x11open_state->channel;
    int rc;

    (void) datalen;

    if(x11open_state->state == libssh2_NB_state_idle) {
        unsigned char *s = data + (sizeof("x11") - 1) + 5;
        x11open_state->sender_channel = _libssh2_ntohu32(s);
        s += 4;
        x11open_state->initial_window_size = _libssh2_ntohu32(s);
        s += 4;
        x11open_state->packet_size = _libssh2_ntohu32(s);
        s += 4;
        x11open_state->shost_len = _libssh2_ntohu32(s);
        s += 4;
        x11open_state->shost = s;
        s += x11open_state->shost_len;
        x11open_state->sport = _libssh2_ntohu32(s);

        _libssh2_debug(session, LIBSSH2_TRACE_CONN,
                       "X11 Connection Received from %s:%ld on channel %lu",
                       x11open_state->shost, x11open_state->sport,
                       x11open_state->sender_channel);

        x11open_state->state = libssh2_NB_state_allocated;
    }

    if(session->x11) {
        if(x11open_state->state == libssh2_NB_state_allocated) {
            channel = LIBSSH2_CALLOC(session, sizeof(LIBSSH2_CHANNEL));
            if(!channel) {
                _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                               "allocate a channel for new connection");
                failure_code = SSH_OPEN_RESOURCE_SHORTAGE;
                goto x11_exit;
            }

            channel->session = session;
            channel->channel_type_len = sizeof("x11") - 1;
            channel->channel_type = LIBSSH2_ALLOC(session,
                                                  channel->channel_type_len +
                                                  1);
            if(!channel->channel_type) {
                _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                               "allocate a channel for new connection");
                LIBSSH2_FREE(session, channel);
                failure_code = SSH_OPEN_RESOURCE_SHORTAGE;
                goto x11_exit;
            }
            memcpy(channel->channel_type, "x11",
                   channel->channel_type_len + 1);

            channel->remote.id = x11open_state->sender_channel;
            channel->remote.window_size_initial =
                LIBSSH2_CHANNEL_WINDOW_DEFAULT;
            channel->remote.window_size = LIBSSH2_CHANNEL_WINDOW_DEFAULT;
            channel->remote.packet_size = LIBSSH2_CHANNEL_PACKET_DEFAULT;

            channel->local.id = _libssh2_channel_nextid(session);
            channel->local.window_size_initial =
                x11open_state->initial_window_size;
            channel->local.window_size = x11open_state->initial_window_size;
            channel->local.packet_size = x11open_state->packet_size;

            _libssh2_debug(session, LIBSSH2_TRACE_CONN,
                           "X11 Connection established: channel %lu/%lu "
                           "win %lu/%lu packet %lu/%lu",
                           channel->local.id, channel->remote.id,
                           channel->local.window_size,
                           channel->remote.window_size,
                           channel->local.packet_size,
                           channel->remote.packet_size);
            p = x11open_state->packet;
            *(p++) = SSH_MSG_CHANNEL_OPEN_CONFIRMATION;
            _libssh2_store_u32(&p, channel->remote.id);
            _libssh2_store_u32(&p, channel->local.id);
            _libssh2_store_u32(&p, channel->remote.window_size_initial);
            _libssh2_store_u32(&p, channel->remote.packet_size);

            x11open_state->state = libssh2_NB_state_created;
        }

        if(x11open_state->state == libssh2_NB_state_created) {
            rc = _libssh2_transport_send(session, x11open_state->packet, 17,
                                         NULL, 0);
            if(rc == LIBSSH2_ERROR_EAGAIN) {
                return rc;
            }
            else if(rc) {
                x11open_state->state = libssh2_NB_state_idle;
                return _libssh2_error(session, LIBSSH2_ERROR_SOCKET_SEND,
                                      "Unable to send channel open "
                                      "confirmation");
            }

            /* Link the channel into the session */
            _libssh2_list_add(&session->channels, &channel->node);

            /*
             * Pass control to the callback, they may turn right around and
             * free the channel, or actually use it
             */
            LIBSSH2_X11_OPEN(channel, (char *)x11open_state->shost,
                             x11open_state->sport);

            x11open_state->state = libssh2_NB_state_idle;
            return 0;
        }
    }
    else
        failure_code = SSH_OPEN_RESOURCE_SHORTAGE;
    /* fall-trough */
  x11_exit:
    p = x11open_state->packet;
    *(p++) = SSH_MSG_CHANNEL_OPEN_FAILURE;
    _libssh2_store_u32(&p, x11open_state->sender_channel);
    _libssh2_store_u32(&p, failure_code);
    _libssh2_store_str(&p, X11FwdUnAvil, sizeof(X11FwdUnAvil) - 1);
    _libssh2_htonu32(p, 0);

    rc = _libssh2_transport_send(session, x11open_state->packet, packet_len,
                                 NULL, 0);
    if(rc == LIBSSH2_ERROR_EAGAIN) {
        return rc;
    }
    else if(rc) {
        x11open_state->state = libssh2_NB_state_idle;
        return _libssh2_error(session, rc, "Unable to send open failure");
    }
    x11open_state->state = libssh2_NB_state_idle;
    return 0;
}