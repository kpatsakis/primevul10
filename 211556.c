Variant HHVM_FUNCTION(pfsockopen,
                      const String& hostname,
                      int port,
                      Variant& errnum,
                      Variant& errstr,
                      double timeout /* = -1.0 */) {
  HostURL hosturl(static_cast<const std::string>(hostname), port);
  return sockopen_impl(hosturl, errnum, errstr, timeout, true, uninit_variant);
}