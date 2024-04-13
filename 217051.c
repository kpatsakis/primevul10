  static inline ShapeArray TensorShapeToVector(const TensorShape& shape) {
    ShapeArray vec(shape.dims());
    for (int i = 0; i < shape.dims(); ++i) vec[i] = shape.dim_size(i);
    return vec;
  }