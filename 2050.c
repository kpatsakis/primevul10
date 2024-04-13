int Socket::bind(const std::string &ip, int port) {
    int len = sizeof my_adr;
    int i = 1;

    setsockopt(sck, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));

    my_adr.sin_port = htons(port);
    my_adr.sin_addr.s_addr = inet_addr(ip.c_str());
    my_port = port;

    return ::bind(sck, (struct sockaddr *) &my_adr, len);
}