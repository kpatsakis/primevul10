inline int MatchingFlatSize(const Dims<N>& dims, const Dims<N>& check_dims_0,
                            const Dims<N>& check_dims_1,
                            const Dims<N>& check_dims_2,
                            const Dims<N>& check_dims_3) {
  for (int i = 0; i < N; ++i) {
    TFLITE_DCHECK_EQ(ArraySize(dims, i), ArraySize(check_dims_0, i));
  }
  return MatchingFlatSize(dims, check_dims_1, check_dims_2, check_dims_3);
}