inline T RowMax(const typename TTypes<T>::UnalignedConstMatrix& m, int r,
                int* c) {
  *c = 0;
  CHECK_LT(0, m.dimension(1));
  auto p = m(r, 0);
  for (int i = 1; i < m.dimension(1); ++i) {
    if (m(r, i) > p) {
      p = m(r, i);
      *c = i;
    }
  }
  return p;
}