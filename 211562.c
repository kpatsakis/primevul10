Variant HHVM_FUNCTION(socket_recv,
                      const Resource& socket,
                      Variant& buf,
                      int64_t len,
                      int flags) {
  if (len <= 0) {
    return false;
  }
  auto sock = cast<Socket>(socket);

  char *recv_buf = (char *)malloc(len + 1);
  int retval;
  if ((retval = recv(sock->fd(), recv_buf, (size_t)len, flags)) < 1) {
    free(recv_buf);
    buf = init_null();
  } else {
    recv_buf[retval] = '\0';
    buf = String(recv_buf, retval, AttachString);
  }

  if (retval == -1) {
    SOCKET_ERROR(sock, "unable to read from socket", errno);
    return false;
  }
  return retval;
}