MatcherConstPtr Matcher::create(const RequirementRule& rule) {
  switch (rule.match().path_specifier_case()) {
  case RouteMatch::PathSpecifierCase::kPrefix:
    return std::make_unique<PrefixMatcherImpl>(rule);
  case RouteMatch::PathSpecifierCase::kPath:
    return std::make_unique<PathMatcherImpl>(rule);
  case RouteMatch::PathSpecifierCase::kSafeRegex:
    return std::make_unique<RegexMatcherImpl>(rule);
  case RouteMatch::PathSpecifierCase::kConnectMatcher:
    return std::make_unique<ConnectMatcherImpl>(rule);
  case RouteMatch::PathSpecifierCase::PATH_SPECIFIER_NOT_SET:
    break; // Fall through to PANIC.
  }
  PANIC_DUE_TO_CORRUPT_ENUM;
}