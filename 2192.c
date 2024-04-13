  bool AllInputValuesKnown(NodeContext* c) {
    InferenceContext* ic = c->inference_context.get();

    // Check inputs are fully defined and values are known.
    for (int i = 0; i < ic->num_inputs(); i++) {
      const Tensor* tensor = ic->input_tensor(i);
      // Note that we don't check c->input_tensor_protos[i], as UpdateNode()
      // already converted it to ic->input_tensor(i);
      const ShapeHandle& input_tensors_as_shape =
          ic->input_tensors_as_shapes()[i];
      // Either input_tensor is valid or input_tensors_as_shape, which has
      // value of input tensors as shape format, should be fully defined.
      if (tensor == nullptr && !ic->FullyDefined(input_tensors_as_shape)) {
        return false;
      }
    }
    return true;
  }