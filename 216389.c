  RuntimeShape(int dimensions_count, const int32_t* dims_data) : size_(0) {
    ReplaceWith(dimensions_count, dims_data);
  }