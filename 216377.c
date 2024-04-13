  inline void ReplaceWith(int dimensions_count, const int32_t* dims_data) {
    Resize(dimensions_count);
    int32_t* dst_dims = DimsData();
    std::memcpy(dst_dims, dims_data, dimensions_count * sizeof(int32_t));
  }