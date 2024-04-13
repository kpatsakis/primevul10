  RuntimeShape(int shape_size, int32_t value) : size_(0) {
    Resize(shape_size);
    for (int i = 0; i < shape_size; ++i) {
      SetDim(i, value);
    }
  }