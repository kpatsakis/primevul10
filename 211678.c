Variant sockopen_impl(const HostURL &hosturl, Variant& errnum,
                      Variant& errstr, double timeout, bool persistent,
                      const Variant& context) {
  errnum = 0;
  errstr = empty_string();
  std::string key;
  if (persistent) {
    key = hosturl.getHostURL() + ":" +
          folly::to<std::string>(hosturl.getPort());

    // Check our persistent storage and determine if it's an SSLSocket
    // or just a regular socket.
    auto sockItr = s_sockets->find(key);
    if (sockItr != s_sockets->end()) {
      req::ptr<Socket> sock;
      if (auto sslSocketData =
          std::dynamic_pointer_cast<SSLSocketData>(sockItr->second)) {
        sock = req::make<SSLSocket>(sslSocketData);
      } else {
        sock = req::make<StreamSocket>(sockItr->second);
      }

      if (sock->getError() == 0 && sock->checkLiveness()) {
        return Variant(sock);
      }

      // socket had an error earlier, we need to close it, remove it from
      // persistent storage, and create a new one (in that order)
      sock->close();
      s_sockets->erase(sockItr);
    }
  }

  if (timeout < 0) {
    timeout = RequestInfo::s_requestInfo.getNoCheck()->
      m_reqInjectionData.getSocketDefaultTimeout();
  }


  req::ptr<StreamContext> streamctx;
  if (context.isResource()) {
    streamctx = cast<StreamContext>(context.toResource());
  }
  auto socket = new_socket_connect(hosturl, timeout, streamctx, errnum, errstr);
  if (!socket.isResource()) {
    return false;
  }

  if (persistent) {
    assertx(!key.empty());
    (*s_sockets)[key] = cast<Socket>(socket)->getData();
    assertx((*s_sockets)[key]);
  }

  return socket;
}