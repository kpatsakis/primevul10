inline tflite::Dims<4> ToRuntimeDims(const tflite::RuntimeShape& array_shape) {
  tflite::Dims<4> result;
  const int dimensions_count = array_shape.DimensionsCount();
  TFLITE_CHECK_LE(dimensions_count, 4);
  int cum_prod = 1;
  for (int i = 0; i < 4; i++) {
    const int new_dim =
        (i < dimensions_count) ? array_shape.Dims(dimensions_count - 1 - i) : 1;
    result.sizes[i] = new_dim;
    result.strides[i] = cum_prod;
    cum_prod *= new_dim;
  }
  return result;
}