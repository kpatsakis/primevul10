inline bool SparseTensor::ToDense(Tensor* out, bool initialize) {
  if (!ValidateAndInitializeToDense<T>(out, initialize)) return false;

  auto out_t = out->flat<T>();
  auto vals_t = vals_.vec<T>();
  auto ix_t = ix_.matrix<int64>();
  const int64* const ix_ptr = ix_t.data();

  if (dims_ == 1) {
    // Fast path for sparse vectors.
    const int64 out_length = out->shape().dim_size(0);
    for (int n = 0; n < vals_t.dimension(0); ++n) {
      const int64 index = internal::SubtleMustCopy(ix_ptr[n]);
      if (!FastBoundsCheck(index, out_length)) return false;
      out_t(index) = vals_t(n);
    }
    return true;
  } else if (dims_ == 2) {
    // Fast path for sparse matrices.
    const auto& out_shape = out->shape();
    const int64 out_rows = out_shape.dim_size(0);
    const int64 out_cols = out_shape.dim_size(1);
    for (int n = 0; n < vals_t.dimension(0); ++n) {
      const int64 row_index = internal::SubtleMustCopy(ix_ptr[n * 2]);
      const int64 col_index = internal::SubtleMustCopy(ix_ptr[n * 2 + 1]);
      if (!(FastBoundsCheck(row_index, out_rows) &&
            FastBoundsCheck(col_index, out_cols))) {
        return false;
      }
      out_t(row_index * out_cols + col_index) = vals_t(n);
    }
    return true;
  } else {
    // General path for N-dimensional sparse tensors.
    gtl::InlinedVector<int64, 4> strides(dims_);
    const auto& out_shape = out->shape().dim_sizes();
    if (dims_ > 0) {
      strides[dims_ - 1] = 1;
    }
    for (int d = dims_ - 2; d >= 0; --d) {
      strides[d] = strides[d + 1] * out_shape[d + 1];
    }

    for (int n = 0; n < vals_t.dimension(0); ++n) {
      bool invalid_dims = false;
      int64 ix = 0;
      for (int d = 0; d < dims_; ++d) {
        const int64 ix_n_d = internal::SubtleMustCopy(ix_ptr[n * dims_ + d]);
        if (!FastBoundsCheck(ix_n_d, out_shape[d])) {
          invalid_dims = true;
        }
        ix += strides[d] * ix_n_d;
      }
      if (invalid_dims) return false;
      out_t(ix) = vals_t(n);
    }
    return true;
  }
}