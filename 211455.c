static int php_read(req::ptr<Socket> sock, void *buf, int64_t maxlen,
                    int flags) {
  int m = fcntl(sock->fd(), F_GETFL);
  if (m < 0) {
    return m;
  }
  int nonblock = (m & O_NONBLOCK);
  m = 0;

  char *t = (char *)buf;
  *t = '\0';
  int n = 0;
  int no_read = 0;
  while (*t != '\n' && *t != '\r' && n < maxlen) {
    if (m > 0) {
      t++;
      n++;
    } else if (m == 0) {
      no_read++;
      if (nonblock && no_read >= 2) {
        return n;
        /* The first pass, m always is 0, so no_read becomes 1
         * in the first pass. no_read becomes 2 in the second pass,
         * and if this is nonblocking, we should return.. */
      }

      if (no_read > 200) {
        errno = ECONNRESET;
        return -1;
      }
    }

    if (n < maxlen) {
      m = recv(sock->fd(), (void *)t, 1, flags);
    }

    if (errno != 0 && errno != ESPIPE && errno != EAGAIN) {
      return -1;
    }
    errno = 0;
  }

  if (n < maxlen) {
    n++;
    /* The only reasons it makes it to here is
     * if '\n' or '\r' are encountered. So, increase
     * the return by 1 to make up for the lack of the
     * '\n' or '\r' in the count (since read() takes
     * place at the end of the loop..) */
  }

  return n;
}