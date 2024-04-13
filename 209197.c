SockTrigger(NS_SOCKET sock)
{
    if (send(sock, NS_EMPTY_STRING, 1, 0) != 1) {
        const char *errstr = ns_sockstrerror(ns_sockerrno);

        Ns_Log(Error, "driver: trigger send() failed: %s", errstr);
    }
}