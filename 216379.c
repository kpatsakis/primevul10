  RuntimeShape(RuntimeShape const& other) : size_(other.DimensionsCount()) {
    if (size_ > kMaxSmallSize) {
      dims_pointer_ = new int32_t[size_];
    }
    std::memcpy(DimsData(), other.DimsData(), sizeof(int32_t) * size_);
  }