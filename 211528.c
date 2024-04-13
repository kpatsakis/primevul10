void WuManberReplacement::initTables() {
  size_t max_shift = m - B + 1;
  hash.fill(-1);
  shift.fill(max_shift);
  prefix.reserve(patterns.size());

  strtr_compare_hash_suffix comparator(m, B);
  std::sort(patterns.begin(), patterns.end(), comparator);

  {
    uint16_t last_h = -1; // assumes not all bits are used
    // patterns is already ordered by hash.
    // Make hash[h] de index of the first pattern in
    // patterns that has hash
    int size = patterns.size();
    for(int i = 0; i != size; ++i) {
      // init hash tab
      uint16_t h = patterns[i].hash(m - B, B) & HASH_TAB_MASK;
      if (h != last_h) {
        hash[h] = i;
        last_h = h;
      }
      // init shift tab
      for (int j = 0; j < max_shift; j++) {
        uint16_t h2 = patterns[i].hash( j, B ) & SHIFT_TAB_MASK;
        assertx((long long) m - (long long) j - B >= 0);
        shift[h2] = MIN(shift[h2], m - j - B);
      }
      // init prefix
      prefix.push_back(patterns[i].hash(0, Bp));
    }
  }

  hash[HASH_TAB_SIZE] = patterns.size();  // OK, we allocated SIZE+1
  for (int i = HASH_TAB_SIZE - 1; i >= 0; i--) {
    if (hash[i] == -1) {
      hash[i] = hash[i + 1];
    }
  }
}