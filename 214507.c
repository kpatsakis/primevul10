  PathMatcherImpl(const RequirementRule& rule)
      : BaseMatcherImpl(rule), path_(rule.match().path()),
        path_matcher_(Matchers::PathMatcher::createExact(path_, !case_sensitive_)) {}