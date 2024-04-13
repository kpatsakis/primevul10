_libssh2_packet_require(LIBSSH2_SESSION * session, unsigned char packet_type,
                        unsigned char **data, size_t *data_len,
                        int match_ofs,
                        const unsigned char *match_buf,
                        size_t match_len,
                        packet_require_state_t *state)
{
    if(state->start == 0) {
        if(_libssh2_packet_ask(session, packet_type, data, data_len,
                                match_ofs, match_buf,
                                match_len) == 0) {
            /* A packet was available in the packet brigade */
            return 0;
        }

        state->start = time(NULL);
    }

    while(session->socket_state == LIBSSH2_SOCKET_CONNECTED) {
        int ret = _libssh2_transport_read(session);
        if(ret == LIBSSH2_ERROR_EAGAIN)
            return ret;
        else if(ret < 0) {
            state->start = 0;
            /* an error which is not just because of blocking */
            return ret;
        }
        else if(ret == packet_type) {
            /* Be lazy, let packet_ask pull it out of the brigade */
            ret = _libssh2_packet_ask(session, packet_type, data, data_len,
                                      match_ofs, match_buf, match_len);
            state->start = 0;
            return ret;
        }
        else if(ret == 0) {
            /* nothing available, wait until data arrives or we time out */
            long left = LIBSSH2_READ_TIMEOUT - (long)(time(NULL) -
                                                      state->start);

            if(left <= 0) {
                state->start = 0;
                return LIBSSH2_ERROR_TIMEOUT;
            }
            return -1; /* no packet available yet */
        }
    }

    /* Only reached if the socket died */
    return LIBSSH2_ERROR_SOCKET_DISCONNECT;
}