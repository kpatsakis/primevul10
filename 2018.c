std::string Socket::getLocalIP() {
    char res[INET_ADDRSTRLEN];
    return inet_ntop(AF_INET,&my_adr.sin_addr, res, sizeof(res));
}