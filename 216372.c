void ComputeStrides(Dims<N>* dims) {
  dims->strides[0] = 1;
  for (int d = 1; d < N; d++) {
    dims->strides[d] = dims->strides[d - 1] * dims->sizes[d - 1];
  }
}