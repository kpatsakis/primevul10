_libssh2_packet_burn(LIBSSH2_SESSION * session,
                     libssh2_nonblocking_states * state)
{
    unsigned char *data;
    size_t data_len;
    unsigned char i, all_packets[255];
    int ret;

    if(*state == libssh2_NB_state_idle) {
        for(i = 1; i < 255; i++) {
            all_packets[i - 1] = i;
        }
        all_packets[254] = 0;

        if(_libssh2_packet_askv(session, all_packets, &data, &data_len, 0,
                                 NULL, 0) == 0) {
            i = data[0];
            /* A packet was available in the packet brigade, burn it */
            LIBSSH2_FREE(session, data);
            return i;
        }

        _libssh2_debug(session, LIBSSH2_TRACE_TRANS,
                       "Blocking until packet becomes available to burn");
        *state = libssh2_NB_state_created;
    }

    while(session->socket_state == LIBSSH2_SOCKET_CONNECTED) {
        ret = _libssh2_transport_read(session);
        if(ret == LIBSSH2_ERROR_EAGAIN) {
            return ret;
        }
        else if(ret < 0) {
            *state = libssh2_NB_state_idle;
            return ret;
        }
        else if(ret == 0) {
            /* FIXME: this might busyloop */
            continue;
        }

        /* Be lazy, let packet_ask pull it out of the brigade */
        if(0 ==
            _libssh2_packet_ask(session, (unsigned char)ret,
                                         &data, &data_len, 0, NULL, 0)) {
            /* Smoke 'em if you got 'em */
            LIBSSH2_FREE(session, data);
            *state = libssh2_NB_state_idle;
            return ret;
        }
    }

    /* Only reached if the socket died */
    return LIBSSH2_ERROR_SOCKET_DISCONNECT;
}