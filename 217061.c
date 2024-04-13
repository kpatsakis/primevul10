  SparseTensor(Tensor ix, Tensor vals, const TensorShape& shape)
      : SparseTensor(std::move(ix), std::move(vals), TensorShapeToVector(shape),
                     UndefinedOrder(TensorShapeToVector(shape))) {}