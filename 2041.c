bool Socket::writeToSocket(const char *buff, int len, unsigned int flags, int timeout, bool check_first, bool honour_reloadconfig)
{
    if (len == 0)   // nothing to write
        return true;
    if (!isssl) {
        return BaseSocket::writeToSocket(buff, len, flags, timeout, check_first, honour_reloadconfig);
    }

    int actuallysent = 0;
    int sent;
    while (actuallysent < len) {
       if (check_first) {
    //        try {
                if(!breadyForOutput(timeout))
                   return false;
     //       } catch (std::exception &e) {
      //          return false;
       //     }
        }
        ERR_clear_error();
        sent = SSL_write(ssl, buff + actuallysent, len - actuallysent);
        if (sent < 0) {
    //        if (errno == EINTR ) {
    //            continue; // was interupted by signal so restart
    //        }
            s_errno = errno;
            String serr(s_errno);
            log_ssl_errors("ssl_write failed - error ",serr.c_str());
            return false;
        }
        if (sent == 0) {
            ishup = true;
            return false; // other end is closed
        }
        actuallysent += sent;
    }
    return true;
}