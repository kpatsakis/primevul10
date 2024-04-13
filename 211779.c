bool socket_create_pair_impl(int domain, int type, int protocol, Variant& fd,
                             bool asStream) {
  check_socket_parameters(domain, type);

  int fds_array[2];
  if (socketpair(domain, type, protocol, fds_array) != 0) {
    SOCKET_ERROR(req::make<StreamSocket>(),
                 "unable to create socket pair",
                 errno);
    return false;
  }

  if (asStream) {
    fd = make_varray(
      Variant(req::make<StreamSocket>(fds_array[0], domain, nullptr, 0, 0.0,
                                      s_socktype_generic)),
      Variant(req::make<StreamSocket>(fds_array[1], domain, nullptr, 0, 0.0,
                                      s_socktype_generic))
    );
  } else {
    fd = make_varray(
      Variant(req::make<ConcreteSocket>(fds_array[0], domain, nullptr, 0, 0.0,
                                        s_socktype_generic)),
      Variant(req::make<ConcreteSocket>(fds_array[1], domain, nullptr, 0, 0.0,
                                        s_socktype_generic))
    );
  }
  return true;
}