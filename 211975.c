  template<size_t N> explicit StaticString(const char(&s)[N]) {
    construct(s, N - 1);
  }