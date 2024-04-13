inline int MatchingFlatSizeSkipDim(const Dims<N>& dims, int skip_dim,
                                   const Dims<N>& check_dims_0) {
  for (int i = 0; i < N; ++i) {
    if (i != skip_dim) {
      TFLITE_DCHECK_EQ(ArraySize(dims, i), ArraySize(check_dims_0, i));
    }
  }
  return FlatSizeSkipDim(dims, skip_dim);
}