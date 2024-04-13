bool Socket::checkForInput()
{
    if (!isssl) {
        return BaseSocket::checkForInput();
    }
#ifdef NETDEBUG
    std::cout << thread_id << "checking for input on ssl connection (non blocking)" << std::endl;
#endif
    if ((bufflen - buffstart) > 0) {
#ifdef NETDEBUG
        std::cout << thread_id << "found input on ssl connection" << std::endl;
#endif
        return true;
    }

    if (!BaseSocket::checkForInput())
        return false;

    //see if we can do an ssl read of 1 byte
//    char buf[1];

    //int rc = SSL_peek(ssl, buf, 1);
    int rc = SSL_pending(ssl);

    if (rc < 1) {
#ifdef NETDEBUG
        std::cout << thread_id << "no pending data on ssl connection SSL_pending " << rc << std::endl;
#endif
        return false;
    }

#ifdef NETDEBUG
    std::cout << thread_id << "found data on ssl connection" << std::endl;
#endif

    return true;
}