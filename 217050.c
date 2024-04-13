  static inline int GetDimensionInSlice(const int dim, const int split_size,
                                        const int residual) {
    DCHECK_GT(split_size, 0);
    DCHECK_GE(dim, 0);
    if (residual == 0) return dim % split_size;
    const int offset = residual * (split_size + 1);
    if (dim < offset) {
      return dim % (split_size + 1);
    } else {
      return (dim - offset) % split_size;
    }
  }