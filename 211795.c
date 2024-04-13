static req::ptr<Socket> create_new_socket(
  const HostURL &hosturl,
  Variant& errnum,
  Variant& errstr,
  const Variant& context
) {
  int domain = hosturl.isIPv6() ? AF_INET6 : AF_INET;
  int type = SOCK_STREAM;
  const std::string scheme = hosturl.getScheme();

  if (scheme == "udp" || scheme == "udg") {
    type = SOCK_DGRAM;
  }

  if (scheme == "unix" || scheme == "udg") {
    domain = AF_UNIX;
  }

  req::ptr<Socket> sock;
  int fd = socket(domain, type, 0);
  double timeout = RequestInfo::s_requestInfo.getNoCheck()->
      m_reqInjectionData.getSocketDefaultTimeout();
  req::ptr<StreamContext> streamctx;
  if (context.isResource()) {
    streamctx = cast<StreamContext>(context.toResource());
  }

  auto sslsock = SSLSocket::Create(fd, domain, hosturl, timeout, streamctx);
  if (sslsock) {
    sock = sslsock;
  } else {
    sock = req::make<StreamSocket>(fd, domain, hosturl.getHost().c_str(),
                                   hosturl.getPort());
  }

  if (!sock->valid()) {
    SOCKET_ERROR(sock, "unable to create socket", errno);
    errnum = sock->getError();
    errstr = HHVM_FN(socket_strerror)(sock->getError());
    sock.reset();
  }
  return sock;
}