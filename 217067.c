  SparseTensor(Tensor ix, Tensor vals, const TensorShape& shape,
               const VarDimArray order)
      : SparseTensor(std::move(ix), std::move(vals), TensorShapeToVector(shape),
                     order) {}