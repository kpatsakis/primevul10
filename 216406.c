  inline int32_t Dims(int i) const {
    TFLITE_DCHECK_GE(i, 0);
    TFLITE_DCHECK_LT(i, size_);
    return size_ > kMaxSmallSize ? dims_pointer_[i] : dims_[i];
  }