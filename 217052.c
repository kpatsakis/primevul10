inline SparseTensor SparseTensor::Slice(const SparseTensor& input_tensor,
                                        const gtl::ArraySlice<int64>& start,
                                        const gtl::ArraySlice<int64>& size) {
  TensorShape output_shape(input_tensor.shape());

  const int dims = input_tensor.dims();
  for (int dim = 0; dim < dims; dim++) {
    // Determine the size of the result; if the selected slice goes beyond the
    // input boundary, the result will correspond to the size of the overlap
    // between the input and the selected slice.
    const int64 input_size = output_shape.dim_size(dim);
    const int64 start_index = start[dim];
    const int64 slice_size = size[dim];
    if (start_index + slice_size < input_size) {
      // The entire selection is within input boundaries.
      output_shape.set_dim(dim, slice_size);
    } else if (start_index < input_size) {
      // The selection starts within input boundaries, but goes beyond them.
      output_shape.set_dim(dim, input_size - start_index);
    } else {
      // The selection is entirely out of input boundaries.
      output_shape.set_dim(dim, 0);
    }
  }

  auto input_indices_t = input_tensor.indices().matrix<int64>();
  auto input_values_t = input_tensor.values().vec<T>();

  // Find the number of indices that fall inside start and size.
  int count = 0;
  for (int i = 0; i < input_tensor.indices().dim_size(0); i++) {
    // The following will check to see if an input is within the
    // range specified by start and size.
    // The for loop below iterates through all dimensions. In case
    // the index falls outside of the start and size at any dimension,
    // it will be considered as a "no hit" (hit = false). In this
    // case, it will not be counted as the index that fall inside
    // the range specified by start and size.
    bool hit = true;
    for (int dim = 0; dim < dims; dim++) {
      if (!(start[dim] <= input_indices_t(i, dim) &&
            input_indices_t(i, dim) < start[dim] + size[dim])) {
        hit = false;
        break;
      }
    }
    if (!hit) {
      continue;
    }
    count++;
  }

  Tensor output_values(DataTypeToEnum<T>::v(), TensorShape({count}));
  Tensor output_indices(DT_INT64, TensorShape({count, dims}));

  auto output_values_t = output_values.vec<T>();
  auto output_indices_t = output_indices.matrix<int64>();

  // Obtain the output indices that fall inside start and size.
  int index = 0;
  for (int i = 0; i < input_tensor.indices().dim_size(0) && index < count;
       i++) {
    // The logic here is similar as the above except that the above
    // only count the number of indices while here we actually generate
    // the output.
    bool hit = true;
    for (int dim = 0; dim < dims; dim++) {
      if (!(start[dim] <= input_indices_t(i, dim) &&
            input_indices_t(i, dim) < start[dim] + size[dim])) {
        hit = false;
        break;
      }
    }
    if (!hit) {
      continue;
    }
    output_values_t(index) = input_values_t(i);
    for (int dim = 0; dim < dims; dim++) {
      output_indices_t(index, dim) = input_indices_t(i, dim) - start[dim];
    }
    index++;
  }

  return SparseTensor(output_indices, output_values, output_shape);
}