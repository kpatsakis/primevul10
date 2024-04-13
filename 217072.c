  SparseTensor(Tensor ix, Tensor vals, const VarDimArray shape)
      : SparseTensor(std::move(ix), std::move(vals), shape,
                     UndefinedOrder(shape)) {}