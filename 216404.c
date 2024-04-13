inline int MatchingFlatSize(const RuntimeShape& shape,
                            const RuntimeShape& check_shape_0,
                            const RuntimeShape& check_shape_1,
                            const RuntimeShape& check_shape_2,
                            const RuntimeShape& check_shape_3) {
  TFLITE_DCHECK_EQ(shape.DimensionsCount(), check_shape_0.DimensionsCount());
  const int dims_count = shape.DimensionsCount();
  for (int i = 0; i < dims_count; ++i) {
    TFLITE_DCHECK_EQ(shape.Dims(i), check_shape_0.Dims(i));
  }
  return MatchingFlatSize(shape, check_shape_1, check_shape_2, check_shape_3);
}