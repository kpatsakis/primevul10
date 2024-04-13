void HHVM_FUNCTION(socket_close,
                   const Resource& socket) {
  cast<Socket>(socket)->close();
}