Variant HHVM_FUNCTION(socket_server,
                      const String& hostname,
                      int port,
                      Variant& errnum,
                      Variant& errstr) {
  HostURL hosturl(static_cast<const std::string>(hostname), port);
  return socket_server_impl(hosturl,
                            k_STREAM_SERVER_BIND|k_STREAM_SERVER_LISTEN,
                            errnum, errstr);
}