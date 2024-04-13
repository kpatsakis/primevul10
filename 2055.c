Socket *Socket::accept() {
    peer_adr_length = sizeof(struct sockaddr_in);
    s_errno = 0;
    errno = 0;
//    int newfd = this->baseAccept((struct sockaddr *)&peer_adr, &peer_adr_length);
    int newfd = ::accept(sck, (struct sockaddr *) &peer_adr, &peer_adr_length);

    if (newfd > 0) {
        Socket *s = new Socket(newfd, my_adr, peer_adr);
        s->setPort(my_port);
        return s;
    } else {
        s_errno = errno;
        return NULL;
    }
}