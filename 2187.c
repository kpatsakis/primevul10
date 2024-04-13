  bool AllOutputValuesKnown(NodeContext* c) {
    InferenceContext* ic = c->inference_context.get();
    int c_output_tensors_as_shapes_size = c->output_tensors_as_shapes.size();
    int c_output_tensor_protos_size = c->output_tensor_protos.size();
    if (c_output_tensors_as_shapes_size < ic->num_outputs() &&
        c_output_tensor_protos_size < ic->num_outputs()) {
      return false;
    } else {
      // Checks if we can get output value via either output_tensor_proto or
      // output_tensors_as_shapes.
      for (int i = 0; i < ic->num_outputs(); i++) {
        if (c_output_tensor_protos_size > i &&
            c->output_tensor_protos[i] != nullptr) {
          continue;
        }
        if (c_output_tensors_as_shapes_size > i &&
            ic->FullyDefined(c->output_tensors_as_shapes[i])) {
          bool no_unknown_dim_from_const = true;
          for (int32_t j = 0; j < ic->Rank(c->output_tensors_as_shapes[i]);
               ++j) {
            const auto dim = ic->Dim(c->output_tensors_as_shapes[i], j);
            if (ic->ValueKnown(dim) && ic->Value(dim) == kUnknownDimFromConst) {
              no_unknown_dim_from_const = false;
              break;
            }
          }
          if (no_unknown_dim_from_const) {
            continue;
          }
        }
        return false;
      }
    }
    return true;
  }