_libssh2_packet_requirev(LIBSSH2_SESSION *session,
                         const unsigned char *packet_types,
                         unsigned char **data, size_t *data_len,
                         int match_ofs,
                         const unsigned char *match_buf, size_t match_len,
                         packet_requirev_state_t * state)
{
    if(_libssh2_packet_askv(session, packet_types, data, data_len, match_ofs,
                             match_buf, match_len) == 0) {
        /* One of the packets listed was available in the packet brigade */
        state->start = 0;
        return 0;
    }

    if(state->start == 0) {
        state->start = time(NULL);
    }

    while(session->socket_state != LIBSSH2_SOCKET_DISCONNECTED) {
        int ret = _libssh2_transport_read(session);
        if((ret < 0) && (ret != LIBSSH2_ERROR_EAGAIN)) {
            state->start = 0;
            return ret;
        }
        if(ret <= 0) {
            long left = LIBSSH2_READ_TIMEOUT -
                (long)(time(NULL) - state->start);

            if(left <= 0) {
                state->start = 0;
                return LIBSSH2_ERROR_TIMEOUT;
            }
            else if(ret == LIBSSH2_ERROR_EAGAIN) {
                return ret;
            }
        }

        if(strchr((char *) packet_types, ret)) {
            /* Be lazy, let packet_ask pull it out of the brigade */
            return _libssh2_packet_askv(session, packet_types, data,
                                        data_len, match_ofs, match_buf,
                                        match_len);
        }
    }

    /* Only reached if the socket died */
    state->start = 0;
    return LIBSSH2_ERROR_SOCKET_DISCONNECT;
}