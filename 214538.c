  bool matches(const Http::RequestHeaderMap& headers) const override {
    if (Http::HeaderUtility::isConnect(headers) && BaseMatcherImpl::matchRoute(headers)) {
      ENVOY_LOG(debug, "CONNECT requirement matched.");
      return true;
    }

    return false;
  }