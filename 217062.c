  std::vector<int64> PickDims(gtl::ArraySlice<int64> dim_indices) const {
    std::vector<int64> res(dim_indices.size());
    for (size_t i = 0; i < dim_indices.size(); ++i) {
      res[i] = shape_[dim_indices[i]];
    }
    return res;
  }