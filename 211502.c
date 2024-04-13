Variant HHVM_FUNCTION(fsockopen,
                      const String& hostname,
                      int port,
                      Variant& errnum,
                      Variant& errstr,
                      double timeout /* = -1.0 */) {
  HostURL hosturl(static_cast<const std::string>(hostname), port);
  return sockopen_impl(hosturl, errnum, errstr, timeout, false, uninit_variant);
}