  SparseTensor& operator=(const SparseTensor& other) {
    ix_ = other.ix_;
    vals_ = other.vals_;
    shape_ = other.shape_;
    order_ = other.order_;
    dims_ = other.dims_;
    return *this;
  }