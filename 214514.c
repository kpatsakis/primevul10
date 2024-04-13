  RegexMatcherImpl(const RequirementRule& rule)
      : BaseMatcherImpl(rule), regex_str_(rule.match().safe_regex().regex()),
        path_matcher_(Matchers::PathMatcher::createSafeRegex(rule.match().safe_regex())) {
    ASSERT(rule.match().path_specifier_case() ==
           envoy::config::route::v3::RouteMatch::PathSpecifierCase::kSafeRegex);
  }