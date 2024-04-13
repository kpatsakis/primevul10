Socket::Socket() {
    sck = socket(AF_INET, SOCK_STREAM, 0);
    if (sck < 0) {
        s_errno = errno;
    } else {
        memset(&my_adr, 0, sizeof my_adr);
        memset(&peer_adr, 0, sizeof peer_adr);
        my_adr.sin_family = AF_INET;
        peer_adr.sin_family = AF_INET;
        peer_adr_length = sizeof(struct sockaddr_in);
        int f = 1;

        if (sck > 0)
            setsockopt(sck, IPPROTO_TCP, TCP_NODELAY, &f, sizeof(int));

        my_port = 0;
        chunkError = false;

#ifdef __SSLMITM
        ssl = NULL;
        ctx = NULL;
        isssl = false;
        issslserver = false;
#else
        isssl = false;
#endif
    }
}