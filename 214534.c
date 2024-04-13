  BaseMatcherImpl(const RequirementRule& rule)
      : case_sensitive_(PROTOBUF_GET_WRAPPED_OR_DEFAULT(rule.match(), case_sensitive, true)),
        config_headers_(Http::HeaderUtility::buildHeaderDataVector(rule.match().headers())) {
    for (const auto& query_parameter : rule.match().query_parameters()) {
      config_query_parameters_.push_back(
          std::make_unique<Router::ConfigUtility::QueryParameterMatcher>(query_parameter));
    }
  }