  inline const int32_t* DimsData() const {
    return size_ > kMaxSmallSize ? dims_pointer_ : dims_;
  }