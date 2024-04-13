  WuManberReplacement(const Array &arr, size_t minLen)
  : m(minLen), B(MIN(m,2)), Bp(MIN(m,2)),
    valid(initPatterns(arr)) { }