bool HHVM_FUNCTION(socket_set_nonblock,
                   const Resource& socket) {
  return cast<Socket>(socket)->setBlocking(false);
}