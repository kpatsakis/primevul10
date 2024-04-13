Variant HHVM_FUNCTION(socket_create,
                      int domain,
                      int type,
                      int protocol) {
  check_socket_parameters(domain, type);
  int socketId = socket(domain, type, protocol);
  if (socketId == -1) {
    SOCKET_ERROR(req::make<ConcreteSocket>(),
                 "Unable to create socket",
                 errno);
    return false;
  }
  return Variant(req::make<ConcreteSocket>(socketId, domain));
}