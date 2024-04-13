inline int FlatSizeSkipDim(const Dims<N>& dims, int skip_dim) {
  TFLITE_DCHECK(skip_dim >= 0 && skip_dim < N);
  int flat_size = 1;
  for (int i = 0; i < N; ++i) {
    flat_size *= (i == skip_dim) ? 1 : dims.sizes[i];
  }
  return flat_size;
}