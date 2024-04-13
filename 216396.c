inline int FlatSize(const Dims<N>& dims) {
  int flat_size = 1;
  for (int i = 0; i < N; ++i) {
    flat_size *= dims.sizes[i];
  }
  return flat_size;
}