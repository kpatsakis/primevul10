inline int MatchingFlatSize(const Dims<N>& dims, const Dims<N>& check_dims_0) {
  for (int i = 0; i < N; ++i) {
    TFLITE_DCHECK_EQ(ArraySize(dims, i), ArraySize(check_dims_0, i));
  }
  return FlatSize(dims);
}