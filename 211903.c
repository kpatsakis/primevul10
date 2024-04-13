void HHVM_FUNCTION(socket_clear_error,
                   const Variant& socket /* = null */) {
  if (!socket.isNull()) {
    cast<Socket>(socket)->setError(0);
  }
}