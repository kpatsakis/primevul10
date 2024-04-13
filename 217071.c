  static inline int GetSliceShape(const int slice_index, const int split_size,
                                  const int residual) {
    DCHECK_GT(split_size, 0);
    DCHECK_GE(slice_index, 0);
    if (residual == 0) return split_size;
    if (slice_index < residual) {
      return split_size + 1;
    } else {
      return split_size;
    }
  }