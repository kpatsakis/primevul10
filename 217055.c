  SparseTensor& operator=(SparseTensor&& other) {
    ix_ = std::move(other.ix_);
    vals_ = std::move(other.vals_);
    shape_ = std::move(other.shape_);
    order_ = std::move(other.order_);
    dims_ = std::move(other.dims_);
    return *this;
  }