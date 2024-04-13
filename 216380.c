inline int MatchingElementsSize(const RuntimeShape& shape,
                                const RuntimeShape& check_shape_0) {
  const int size_1 = shape.FlatSize();
  const int size_2 = check_shape_0.FlatSize();
  TFLITE_CHECK_EQ(size_1, size_2);
  return size_1;
}