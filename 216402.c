  inline int32_t* DimsData() {
    return size_ > kMaxSmallSize ? dims_pointer_ : dims_;
  }