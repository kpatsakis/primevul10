int Socket::readFromSocketn(char *buff, int len, unsigned int flags, int timeout)
{
    return readFromSocket(buff, len, flags, timeout, true, false);


#ifdef NODEF
    if (!isssl) {
        return BaseSocket::readFromSocketn(buff, len, flags, timeout);
    }

    int cnt, rc;
    cnt = len;

    // first, return what's left from the previous buffer read, if anything
    if ((bufflen - buffstart) > 0) {
#ifdef NETDEBUG
        std::cout << thread_id << "Socket::readFromSocketn: data already in buffer; bufflen: " << bufflen << " buffstart: " << buffstart << std::endl;
#endif
        int tocopy = len;
        if ((bufflen - buffstart) < len)
            tocopy = bufflen - buffstart;
        memcpy(buff, buffer + buffstart, tocopy);
        cnt -= tocopy;
        buffstart += tocopy;
        buff += tocopy;
        if (cnt == 0)
            return len;
    }

    while (cnt > 0) {
    //    try {
            //bcheckSForInput(timeout);        //  this may be wrong - why is data not being read into socket buffer????
    //    } catch (std::exception &e) {
     //       return -1;
     //   }
        ERR_clear_error();
        rc = SSL_read(ssl, buff, cnt);
#ifdef NETDEBUG
        std::cout << thread_id << "ssl read said: " << rc << std::endl;
#endif

        if (rc < 0) {
       //     if (errno == EINTR) {
         //       continue;
           // }
            log_ssl_errors("ssl_read failed %s", "");
           s_errno = errno;
            return -1;
        }
        if (rc == 0) { // eof
             ishup = true;
            return len - cnt;
        }
        buff += rc;
        cnt -= rc;
    }
    return len;
#endif
}