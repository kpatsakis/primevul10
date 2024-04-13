  void CreateInputTensors(NodeContext* c,
                          std::vector<Tensor>* input_tensor_vector,
                          TensorVector* inputs) {
    InferenceContext* ic = c->inference_context.get();
    for (int i = 0; i < ic->num_inputs(); i++) {
      if (ic->input_tensor(i)) {
        input_tensor_vector->at(i) = *ic->input_tensor(i);
        inputs->emplace_back(&input_tensor_vector->at(i));
        // Note that we don't check c->input_tensor_protos[i], as UpdateNode()
        // already converted it to ic->input_tensor(i);
      } else {
        // Create Tensor from input_tensors_as_shapes, and then emplace it
        // back to inputs.
        // Note that input_tensors_as_shapes is scalar or vector.
        const ShapeHandle& shape_handle = ic->input_tensors_as_shapes()[i];
        const DataType& data_type = c->input_types[i];
        int32_t rank = ic->Rank(shape_handle);
        if (rank < 1) {
          input_tensor_vector->at(i) = Tensor(data_type, {});
        } else {
          input_tensor_vector->at(i) = Tensor(data_type, {rank});
        }
        auto* tensor = &input_tensor_vector->at(i);
        if (data_type == DT_INT32) {
          auto flat = tensor->flat<int32>();
          for (int j = 0; j < rank; j++) {
            int32_t dim = ic->Value(ic->Dim(shape_handle, j));
            flat(j) = dim;
          }
        } else {
          auto flat = tensor->flat<int64_t>();
          for (int j = 0; j < rank; j++) {
            int64_t dim = ic->Value(ic->Dim(shape_handle, j));
            flat(j) = dim;
          }
        }
        inputs->emplace_back(tensor);
      }
    }
  }