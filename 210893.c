  void Compute(OpKernelContext* context) override {
    const int64 axis_input = context->input(0).scalar<int64>()();
    const Tensor& input_indices = context->input(1);
    const Tensor& input_values = context->input(2);
    const Tensor& input_shape = context->input(3);

    OP_REQUIRES(context, TensorShapeUtils::IsMatrix(input_indices.shape()),
                errors::InvalidArgument(
                    "Input indices should be a matrix but received shape ",
                    input_indices.shape().DebugString()));
    OP_REQUIRES(context, TensorShapeUtils::IsVector(input_values.shape()),
                errors::InvalidArgument(
                    "Input values should be a vector but received shape ",
                    input_indices.shape().DebugString()));
    OP_REQUIRES(context, TensorShapeUtils::IsVector(input_shape.shape()),
                errors::InvalidArgument(
                    "Input shape should be a vector but received shape ",
                    input_shape.shape().DebugString()));

    const int64 input_rank = input_shape.vec<int64>().size();
    const int64 axis = (axis_input < 0) ? input_rank + axis_input : axis_input;

    OP_REQUIRES(
        context, axis >= 0 && axis < input_rank,
        errors::InvalidArgument("Input axis should be in range [", -input_rank,
                                ", ", input_rank, "), got ", axis_input));

    OP_REQUIRES(context,
                num_split_ >= 1 && num_split_ <= input_shape.vec<int64>()(axis),
                errors::InvalidArgument("Input num_split should be between 1 "
                                        "and the splitting dimension size (",
                                        input_shape.vec<int64>()(axis),
                                        "), got ", num_split_));

    sparse::SparseTensor sparse_tensor;
    OP_REQUIRES_OK(context,
                   sparse::SparseTensor::Create(
                       input_indices, input_values,
                       TensorShape(input_shape.vec<int64>()), &sparse_tensor));

    std::vector<sparse::SparseTensor> outputs;
    OP_REQUIRES_OK(context, sparse::SparseTensor::Split<T>(
                                sparse_tensor, axis, num_split_, &outputs));

    for (int slice_index = 0; slice_index < num_split_; ++slice_index) {
      context->set_output(slice_index, outputs[slice_index].indices());
      context->set_output(slice_index + num_split_,
                          outputs[slice_index].values());
      Tensor* shape = nullptr;
      OP_REQUIRES_OK(context, context->allocate_output(
                                  slice_index + 2 * num_split_,
                                  {outputs[slice_index].dims()}, &shape));
      auto output_shape = outputs[slice_index].shape();
      for (int dim = 0; dim < outputs[slice_index].dims(); ++dim) {
        shape->vec<int64>()(dim) = output_shape[dim];
      }
    }
  }