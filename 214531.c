  bool matches(const Http::RequestHeaderMap& headers) const override {
    if (BaseMatcherImpl::matchRoute(headers) && path_matcher_->match(headers.getPathValue())) {
      ENVOY_LOG(debug, "Path requirement '{}' matched.", path_);
      return true;
    }
    return false;
  }