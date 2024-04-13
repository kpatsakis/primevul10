inline int MatchingElementsSize(const RuntimeShape& shape,
                                const RuntimeShape& check_shape_0,
                                const RuntimeShape& check_shape_1) {
  const int size_1 = shape.FlatSize();
  const int size_2 = check_shape_0.FlatSize();
  const int size_3 = check_shape_1.FlatSize();
  TFLITE_CHECK_EQ(size_1, size_2);
  TFLITE_CHECK_EQ(size_2, size_3);
  return size_1;
}