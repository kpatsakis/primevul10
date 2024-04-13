inline int MatchingFlatSizeSkipDim(const RuntimeShape& shape, int skip_dim,
                                   const RuntimeShape& check_shape_0,
                                   const RuntimeShape& check_shape_1,
                                   const RuntimeShape& check_shape_2) {
  const int dims_count = shape.DimensionsCount();
  for (int i = 0; i < dims_count; ++i) {
    if (i != skip_dim) {
      TFLITE_DCHECK_EQ(shape.Dims(i), check_shape_0.Dims(i));
    }
  }
  return MatchingFlatSizeSkipDim(shape, skip_dim, check_shape_1, check_shape_2);
}