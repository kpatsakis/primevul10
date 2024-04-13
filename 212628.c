_libssh2_packet_ask(LIBSSH2_SESSION * session, unsigned char packet_type,
                    unsigned char **data, size_t *data_len,
                    int match_ofs, const unsigned char *match_buf,
                    size_t match_len)
{
    LIBSSH2_PACKET *packet = _libssh2_list_first(&session->packets);

    _libssh2_debug(session, LIBSSH2_TRACE_TRANS,
                   "Looking for packet of type: %d", (int) packet_type);

    while(packet) {
        if(packet->data[0] == packet_type
            && (packet->data_len >= (match_ofs + match_len))
            && (!match_buf ||
                (memcmp(packet->data + match_ofs, match_buf,
                        match_len) == 0))) {
            *data = packet->data;
            *data_len = packet->data_len;

            /* unlink struct from session->packets */
            _libssh2_list_remove(&packet->node);

            LIBSSH2_FREE(session, packet);

            return 0;
        }
        packet = _libssh2_list_next(&packet->node);
    }
    return -1;
}