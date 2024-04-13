inline int Offset(const RuntimeShape& shape, int i0, int i1, int i2, int i3) {
  TFLITE_DCHECK_EQ(shape.DimensionsCount(), 4);
  const int* dims_data = reinterpret_cast<const int*>(shape.DimsDataUpTo5D());
  TFLITE_DCHECK(i0 >= 0 && i0 < dims_data[0]);
  TFLITE_DCHECK(i1 >= 0 && i1 < dims_data[1]);
  TFLITE_DCHECK(i2 >= 0 && i2 < dims_data[2]);
  TFLITE_DCHECK(i3 >= 0 && i3 < dims_data[3]);
  return ((i0 * dims_data[1] + i1) * dims_data[2] + i2) * dims_data[3] + i3;
}