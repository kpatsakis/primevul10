bool HHVM_FUNCTION(socket_create_pair,
                   int domain,
                   int type,
                   int protocol,
                   Variant& fd) {
  return socket_create_pair_impl(domain, type, protocol, fd, false);
}