inline int Offset(const RuntimeShape& shape, int* index) {
  return Offset(shape, index[0], index[1], index[2], index[3]);
}