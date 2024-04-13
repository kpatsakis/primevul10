  inline static RuntimeShape ExtendedShape(int new_shape_size,
                                           const RuntimeShape& shape) {
    return RuntimeShape(new_shape_size, shape, 1);
  }