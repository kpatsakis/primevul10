void Socket::reset() {
#ifdef __SSLMITM
    if (isssl) {
        stopSsl();
    }
#endif //__SSLMITM
    this->baseReset();

    sck = socket(AF_INET, SOCK_STREAM, 0);
    if (sck < 0) {
        s_errno = errno;
        return;
    }

    memset(&my_adr, 0, sizeof my_adr);
    memset(&peer_adr, 0, sizeof peer_adr);
    my_adr.sin_family = AF_INET;
    peer_adr.sin_family = AF_INET;
    peer_adr_length = sizeof(struct sockaddr_in);
    infds[0].fd = sck;
    outfds[0].fd = sck;

    chunkError = false;
    chunk_to_read = 0;

}