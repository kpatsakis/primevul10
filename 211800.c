int64_t HHVM_FUNCTION(socket_last_error,
                      const Variant& socket /* = null */) {
  if (!socket.isNull()) {
    return cast<Socket>(socket)->getError();
  }
  return Socket::getLastError();
}