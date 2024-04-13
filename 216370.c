  inline void BuildFrom(const T& src_iterable) {
    const int dimensions_count =
        std::distance(src_iterable.begin(), src_iterable.end());
    Resize(dimensions_count);
    int32_t* data = DimsData();
    for (auto it : src_iterable) {
      *data = it;
      ++data;
    }
  }