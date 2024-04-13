  OneHotContext(TfLiteContext* context, TfLiteNode* node) {
    indices = GetInput(context, node, kIndicesTensor);
    depth = GetInput(context, node, kDepthTensor);
    on_value = GetInput(context, node, kOnValueTensor);
    off_value = GetInput(context, node, kOffValueTensor);
    output = GetOutput(context, node, kOutputTensor);

    const auto* params =
        reinterpret_cast<TfLiteOneHotParams*>(node->builtin_data);
    const int indices_dims = indices->dims->size;
    axis = (params->axis == -1) ? indices_dims : params->axis;
    output_dims = indices_dims + 1;
    dtype = on_value->type;
  }