static Variant new_socket_connect(const HostURL &hosturl, double timeout,
                                  const req::ptr<StreamContext>& streamctx,
                                  Variant& errnum, Variant& errstr) {
  int domain = AF_UNSPEC;
  int type = SOCK_STREAM;
  auto const& scheme = hosturl.getScheme();
  req::ptr<Socket> sock;
  req::ptr<SSLSocket> sslsock;
  std::string sockerr;
  int error;

  if (scheme == "udp" || scheme == "udg") {
    type = SOCK_DGRAM;
  }

  if (scheme == "unix" || scheme == "udg") {
    domain = AF_UNIX;
  }

  int fd = -1;
  if (domain == AF_UNIX) {
    sockaddr_storage sa_storage;
    struct sockaddr *sa_ptr;
    size_t sa_size;

    fd = socket(domain, type, 0);
    sock = req::make<StreamSocket>(
      fd, domain, hosturl.getHost().c_str(), hosturl.getPort(),
      0, empty_string_ref, false);

    if (!set_sockaddr(sa_storage, sock, hosturl.getHost(),
                      hosturl.getPort(), sa_ptr, sa_size)) {
      // set_sockaddr raises its own warning on failure
      return false;
    }
    if (connect_with_timeout(fd, sa_ptr, sa_size, timeout,
                             hosturl, sockerr, error) != 0) {
      SOCKET_ERROR(sock, sockerr.c_str(), error);
      errnum = sock->getLastError();
      errstr = HHVM_FN(socket_strerror)(sock->getLastError());
      return false;
    }
  } else {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = domain;
    hints.ai_socktype = type;

    auto port = folly::to<std::string>(hosturl.getPort());
    auto host = hosturl.getHost();

    struct addrinfo *aiHead;
    int errcode = getaddrinfo(host.c_str(), port.c_str(), &hints, &aiHead);
    if (errcode != 0) {
      errstr = String(gai_strerror(errcode), CopyString);
      return false;
    }
    SCOPE_EXIT { freeaddrinfo(aiHead); };

    for (struct addrinfo *ai = aiHead; ai != nullptr; ai = ai->ai_next) {
      domain = ai->ai_family;
      fd = socket(domain, ai->ai_socktype, ai->ai_protocol);
      if (fd == -1) {
        continue;
      }

      if (connect_with_timeout(fd, ai->ai_addr, ai->ai_addrlen, timeout,
                               hosturl, sockerr, error) == 0) {
        break;
      }
      close(fd);
      fd = -1;
    }

    sslsock = SSLSocket::Create(fd, domain, hosturl, timeout, streamctx, false);
    if (sslsock) {
      sock = sslsock;
    } else {
      sock = req::make<StreamSocket>(fd,
                                     domain,
                                     hosturl.getHost().c_str(),
                                     hosturl.getPort(),
                                     0,
                                     empty_string_ref,
                                     false);
    }
  }

  if (!sock->valid()) {
    SOCKET_ERROR(sock,
        sockerr.empty() ? "unable to create socket" : sockerr.c_str(), error);
    errnum = sock->getLastError();
    errstr = HHVM_FN(socket_strerror)(sock->getLastError());
    return false;
  }

  if (sslsock && !sslsock->onConnect()) {
    raise_warning("Failed to enable crypto");
    return false;
  }

  return Variant(std::move(sock));
}