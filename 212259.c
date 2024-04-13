Variant HHVM_FUNCTION(socket_read,
                      const Resource& socket,
                      int64_t length,
                      int type /* = 0 */) {
  if (length <= 0) {
    return false;
  }
  auto sock = cast<Socket>(socket);

  char *tmpbuf = (char *)malloc(length + 1);
  int retval;
  if (type == PHP_NORMAL_READ) {
    retval = php_read(sock, tmpbuf, length, 0);
  } else {
    retval = recv(sock->fd(), tmpbuf, (size_t)length, 0);
  }

  if (retval == -1) {
    /* if the socket is in non-blocking mode and there's no data to read,
    don't output any error, as this is a normal situation, and not an error */
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      sock->setError(errno);
    } else {
      SOCKET_ERROR(sock, "unable to read from socket", errno);
    }

    free(tmpbuf);
    return false;
  }

  tmpbuf[retval] = '\0' ;
  return String(tmpbuf, retval, AttachString);
}