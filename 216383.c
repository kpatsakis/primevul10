inline RuntimeShape DimsToShape(const tflite::Dims<4>& dims) {
  return RuntimeShape(
      {dims.sizes[3], dims.sizes[2], dims.sizes[1], dims.sizes[0]});
}