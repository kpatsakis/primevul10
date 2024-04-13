  SparseTensor(SparseTensor&& other)
      : SparseTensor(std::move(other.ix_), std::move(other.vals_),
                     std::move(other.shape_), std::move(other.order_)) {}