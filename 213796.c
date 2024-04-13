  Status operator()(OpKernelContext* context, const Tensor& default_value_t,
                    const Tensor& indices_t, const Tensor& values_t,
                    const Tensor& dense_shape_t,
                    typename AsyncOpKernel::DoneCallback done) {
    (void)done;  // Unused (only used in GPU implementation)
    const int kOutputIndicesOutput = 0;
    const int kOutputValuesOutput = 1;
    const int kEmptyRowIndicatorOutput = 2;
    const int kReverseIndexMapOutput = 3;

    const T& default_value = default_value_t.scalar<T>()();
    const auto indices = indices_t.matrix<Tindex>();
    const auto values = values_t.vec<T>();
    const auto dense_shape = dense_shape_t.vec<Tindex>();

    const Tindex N = indices_t.shape().dim_size(0);
    const Tindex dense_rows = dense_shape(0);

    bool* empty_row_indicator = nullptr;
    if (context->output_required(kEmptyRowIndicatorOutput)) {
      Tensor* empty_row_indicator_t = nullptr;
      TF_RETURN_IF_ERROR(context->allocate_output(kEmptyRowIndicatorOutput,
                                                  TensorShape({dense_rows}),
                                                  &empty_row_indicator_t));
      empty_row_indicator = empty_row_indicator_t->vec<bool>().data();
    }
    Tindex* reverse_index_map = nullptr;
    if (context->output_required(kReverseIndexMapOutput)) {
      Tensor* reverse_index_map_t = nullptr;
      TF_RETURN_IF_ERROR(context->allocate_output(
          kReverseIndexMapOutput, TensorShape({N}), &reverse_index_map_t));
      reverse_index_map = reverse_index_map_t->vec<Tindex>().data();
    }

    int rank = indices_t.shape().dim_size(1);

    if (dense_rows == 0) {
      if (N != 0) {
        return errors::InvalidArgument(
            "Received SparseTensor with dense_shape[0] = 0 but "
            "indices.shape[0] = ",
            N);
      }
      Tensor* output_indices_t;
      TensorShape output_indices_shape({0, rank});
      TF_RETURN_IF_ERROR(context->allocate_output(
          kOutputIndicesOutput, output_indices_shape, &output_indices_t));
      Tensor* output_values_t;
      TF_RETURN_IF_ERROR(context->allocate_output(
          kOutputValuesOutput, TensorShape({0}), &output_values_t));

      // Exit early, nothing more to do.
      return Status::OK();
    }

    bool rows_are_ordered = true;
    Tindex last_indices_row = 0;
    std::vector<Tindex> csr_offset(dense_rows, 0);
    for (int i = 0; i < N; ++i) {
      const Tindex row = indices(i, 0);
      if (row < 0 || row >= dense_rows) {
        return errors::InvalidArgument("indices(", i, ", 0) is invalid: ", row,
                                       " >= ", dense_rows);
      }
      ++csr_offset[row];
      rows_are_ordered = rows_are_ordered & (row >= last_indices_row);
      last_indices_row = row;
    }
    bool all_rows_full = true;
    for (int row = 0; row < dense_rows; ++row) {
      // csr_offset here describes the number of elements in this dense row
      bool row_empty = (csr_offset[row] == 0);
      if (empty_row_indicator) {
        empty_row_indicator[row] = row_empty;
      }
      all_rows_full = all_rows_full & !row_empty;
      // In filled version, each row has at least one element.
      csr_offset[row] = std::max(csr_offset[row], Tindex{1});
      // Update csr_offset to represent the number of elements up to and
      // including dense_row + 1:
      //  csr_offset(0) == #{elements of row 0}
      //  csr_offset(1) == #{elements of row 1} + #{elements of row 0}
      //  ..
      //  csr_offset(i) == starting index for elements in row i + 1.
      if (row > 0) {
        csr_offset[row] += csr_offset[row - 1];
      }
    }

    if (all_rows_full && rows_are_ordered) {
      context->set_output(kOutputIndicesOutput, indices_t);
      context->set_output(kOutputValuesOutput, values_t);
      if (reverse_index_map) {
        for (Tindex i = 0; i < N; ++i) {
          reverse_index_map[i] = i;
        }
      }
    } else {
      Tensor* output_indices_t;
      const Tindex N_full = csr_offset[dense_rows - 1];
      TensorShape output_indices_shape({N_full, rank});
      TF_RETURN_IF_ERROR(context->allocate_output(
          kOutputIndicesOutput, output_indices_shape, &output_indices_t));
      auto output_indices = output_indices_t->matrix<Tindex>();

      Tensor* output_values_t;
      TF_RETURN_IF_ERROR(context->allocate_output(
          kOutputValuesOutput, TensorShape({N_full}), &output_values_t));
      auto output_values = output_values_t->vec<T>();

      std::vector<Tindex> filled_count(dense_rows, 0);

      // Fill in values for rows that are not missing
      for (Tindex i = 0; i < N; ++i) {
        const Tindex row = indices(i, 0);
        Tindex& offset = filled_count[row];
        const Tindex output_i = ((row == 0) ? 0 : csr_offset[row - 1]) + offset;
        offset++;  // Increment the filled count for this row.
        std::copy_n(&indices(i, 0), rank, &output_indices(output_i, 0));
        output_values(output_i) = values(i);
        // We'll need this reverse index map to backprop correctly.
        if (reverse_index_map) {
          reverse_index_map[i] = output_i;
        }
      }

      // Fill in values for rows that are missing
      for (Tindex row = 0; row < dense_rows; ++row) {
        const Tindex row_count = filled_count[row];
        if (row_count == 0) {  // We haven't filled this row
          const Tindex starting_index = (row == 0) ? 0 : csr_offset[row - 1];
          // Remaining index values were set to zero already.
          // Just need to set the row index in the right location.
          output_indices(starting_index, 0) = row;
          for (Tindex col = 1; col < rank; ++col) {
            output_indices(starting_index, col) = 0;
          }
          output_values(starting_index) = default_value;
        }
      }
    }

    return Status::OK();
  }