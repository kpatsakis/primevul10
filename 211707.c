bool HHVM_FUNCTION(socket_set_block,
                   const Resource& socket) {
  return cast<Socket>(socket)->setBlocking(true);
}