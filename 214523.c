  PrefixMatcherImpl(const RequirementRule& rule)
      : BaseMatcherImpl(rule), prefix_(rule.match().prefix()),
        path_matcher_(Matchers::PathMatcher::createPrefix(prefix_, !case_sensitive_)) {}