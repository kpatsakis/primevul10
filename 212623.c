_libssh2_packet_askv(LIBSSH2_SESSION * session,
                     const unsigned char *packet_types,
                     unsigned char **data, size_t *data_len,
                     int match_ofs,
                     const unsigned char *match_buf,
                     size_t match_len)
{
    int i, packet_types_len = strlen((char *) packet_types);

    for(i = 0; i < packet_types_len; i++) {
        if(0 == _libssh2_packet_ask(session, packet_types[i], data,
                                     data_len, match_ofs,
                                     match_buf, match_len)) {
            return 0;
        }
    }

    return -1;
}