int Socket::getLine(char *buff, int size, int timeout, bool honour_reloadconfig, bool *chopped, bool *truncated)
{
try {
    if (!isssl) {
        return BaseSocket::getLine(buff, size, timeout, honour_reloadconfig, chopped, truncated);
    }

    // first, return what's left from the previous buffer read, if anything
    int i = 0;
    if ((bufflen - buffstart) > 0) {
        /*#ifdef NETDEBUG
        std::cout << thread_id << "data already in buffer; bufflen: " << bufflen << " buffstart: " << buffstart << std::endl;
#endif*/
        int tocopy = size - 1;
        if ((bufflen - buffstart) < tocopy)
            tocopy = bufflen - buffstart;
        char *result = (char *)memccpy(buff, buffer + buffstart, '\n', tocopy);
        if (result != NULL) {
            // indicate that a newline was chopped off, if desired
            if (chopped)
                *chopped = true;
            *(--result) = '\0';
            buffstart += (result - buff) + 1;
            return result - buff;
        } else {
            i += tocopy;
            buffstart += tocopy;
        }
    }
    while (i < (size - 1)) {
        buffstart = 0;
        bufflen = 0;
 //       try {
 //            checkForInput(timeout, honour_reloadconfig);
 //       } catch (std::exception &e) {
//            throw std::runtime_error(std::string("Can't read from socket: ") + strerror(errno)); // on error
 //       }
//        if( bcheckSForInput(timeout))
            bufflen = SSL_read(ssl, buffer, 4096);
#ifdef NETDEBUG
//std::cout << thread_id << "read into buffer; bufflen: " << bufflen <<std::endl;
#endif
        if (bufflen < 0) {
  //          if (errno == EINTR ) {
   //             continue;
    //        }
            std::cout << thread_id << "SSL_read failed with error " << SSL_get_error(ssl, bufflen) << std::endl;
            log_ssl_errors("ssl_read failed %s", "");
            return -1;
//            throw std::runtime_error(std::string("Can't read from ssl socket")); //strerror(errno));  // on error
        }
        //if socket closed...
        if (bufflen == 0) {
            buff[i] = '\0'; // ...terminate string & return what read
            if (truncated)
                *truncated = true;
            return i;
        }
        int tocopy = bufflen;
        if ((i + bufflen) > (size - 1))
            tocopy = (size - 1) - i;
        char *result = (char *)memccpy(buff + i, buffer, '\n', tocopy);
        if (result != NULL) {
            // indicate that a newline was chopped off, if desired
            if (chopped)
                *chopped = true;
            *(--result) = '\0';
            buffstart += (result - (buff + i)) + 1;
            return i + (result - (buff + i));
        }
        i += tocopy;
    }
    // oh dear - buffer end reached before we found a newline
    buff[i] = '\0';
    if (truncated)
        *truncated = true;
    return i;
    } catch (...) {
    return -1;    }
}