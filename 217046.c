inline SparseTensor SparseTensor::Concat(
    const gtl::ArraySlice<SparseTensor>& tensors) {
  DCHECK_GE(tensors.size(), size_t{1}) << "Cannot concat 0 SparseTensors";
  const int dims = tensors[0].dims_;
  DCHECK_GE(dims, 1) << "Cannot concat 0-dimensional SparseTensors";
  auto order_0 = tensors[0].order();
  const int primary_dim = order_0[0];
  ShapeArray final_order(order_0.begin(), order_0.end());
  ShapeArray final_shape(tensors[0].shape().begin(), tensors[0].shape().end());
  final_shape[primary_dim] = 0;  // We'll build this up as we go along.
  int num_entries = 0;

  bool fully_ordered = true;
  for (const SparseTensor& st : tensors) {
    DCHECK_EQ(st.dims_, dims) << "All SparseTensors must have the same rank.";
    DCHECK_EQ(DataTypeToEnum<T>::v(), st.dtype())
        << "Concat requested with the wrong data type";
    DCHECK_GE(st.order()[0], 0) << "SparseTensor must be ordered";
    DCHECK_EQ(st.order()[0], primary_dim)
        << "All SparseTensors' order[0] must match.  This is the concat dim.";
    if (st.order() != final_order) fully_ordered = false;
    const VarDimArray& st_shape = st.shape();
    for (int d = 0; d < dims - 1; ++d) {
      const int cdim = (d < primary_dim) ? d : d + 1;
      DCHECK_EQ(final_shape[cdim], st_shape[cdim])
          << "All SparseTensors' shapes must match except on the concat dim.  "
          << "Concat dim: " << primary_dim
          << ", mismatched shape at dim: " << cdim
          << ".  Expecting shape like: [" << str_util::Join(final_shape, ",")
          << "] but saw shape: [" << str_util::Join(st_shape, ",") << "]";
    }

    // Update dimension of final shape
    final_shape[primary_dim] =
        (final_shape[primary_dim] + st_shape[primary_dim]);

    num_entries += st.num_entries();  // Update number of entries
  }

  // If nonconsistent ordering among inputs, set final order to -1s.
  if (!fully_ordered) {
    final_order = UndefinedOrder(final_shape);
  }

  Tensor output_ix(DT_INT64, TensorShape({num_entries, dims}));
  Tensor output_vals(DataTypeToEnum<T>::v(), TensorShape({num_entries}));

  TTypes<int64>::Matrix ix_t = output_ix.matrix<int64>();
  typename TTypes<T>::Vec vals_t = output_vals.vec<T>();

  Eigen::DenseIndex offset = 0;
  int64 shape_offset = 0;
  for (const SparseTensor& st : tensors) {
    const int st_num_entries = st.num_entries();

    // Fill in indices & values.
    if (st_num_entries > 0) {
      std::copy_n(&st.vals_.vec<T>()(0), st_num_entries, &vals_t(offset));

      const auto* st_ix = &st.ix_.matrix<int64>()(0, 0);
      auto* ix_out = &ix_t(offset, 0);
      for (std::size_t i = 0; i < st_num_entries * dims; ++i) {
        *ix_out++ = *st_ix++ + ((i % dims == primary_dim) ? shape_offset : 0);
      }
    }

    offset += st_num_entries;
    shape_offset += st.shape()[primary_dim];
  }

  return SparseTensor(output_ix, output_vals, final_shape, final_order);
}