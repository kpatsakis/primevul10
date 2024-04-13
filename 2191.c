  Status UpdateNode(const NodeDef* node, bool* refined) {
    NodeContext* ctx = GetNodeContext(node);
    if (ctx == nullptr) {
      TF_RETURN_IF_ERROR(AddNode(node));
      ctx = CHECK_NOTNULL(GetNodeContext(node));
      *refined = true;
    }

    // Check if the shapes of the nodes in the fan-in of this node have changed,
    // and if they have, update the node input shapes.
    InferenceContext* ic = ctx->inference_context.get();
    ctx->input_tensors_as_shapes_to_propagate.resize(ic->num_inputs());
    ctx->input_tensor_protos.resize(ic->num_inputs(), nullptr);

    for (int dst_input = 0; dst_input < ic->num_inputs(); ++dst_input) {
      const GraphView::InputPort port(node, dst_input);
      const GraphView::OutputPort fanin = graph_.GetRegularFanin(port);
      int src_output = fanin.port_id;
      const NodeDef* src = fanin.node;
      NodeContext* src_ctx = GetNodeContext(src);
      if (src_ctx == nullptr) {
        return errors::FailedPrecondition(
            "Input ", dst_input, " for '", node->name(),
            "' was not previously added to SymbolicShapeRefiner.");
      }

      InferenceContext* src_ic = src_ctx->inference_context.get();
      if (src_output >= src_ic->num_outputs()) {
        return errors::OutOfRange("src_output = ", src_output,
                                  ", but num_outputs is only ",
                                  src_ic->num_outputs());
      }

      // Propagate input node's NodeContext info to the current node's
      // NodeContext:
      // output_tensor_protos to input_tensor_protos and input_tensors, and
      // output_tensors_as_shapes to input_tensors_as_shapes.
      if (static_cast<int>(src_ctx->output_tensors_as_shapes.size()) >
          src_output) {
        ctx->input_tensors_as_shapes_to_propagate[dst_input] =
            src_ctx->output_tensors_as_shapes[src_output];
      }

      if (static_cast<int>(src_ctx->output_tensor_protos.size()) > src_output) {
        const auto* tensor_proto = src_ctx->output_tensor_protos[src_output];
        if (tensor_proto != nullptr) {
          ctx->input_tensor_protos[dst_input] = tensor_proto;
        }
      }

      // NOTE: we check only shape is refined; we do not (yet) check whether
      // tensor value is refined.
      if (!*refined &&
          !ic->input(dst_input).SameHandle(src_ic->output(src_output))) {
        *refined = true;
      }
      ic->SetInput(dst_input, src_ic->output(src_output));

      if (!*refined && ic->requested_input_tensor_as_partial_shape(dst_input)) {
        // The input value may have changed. Since we have no way to know if
        // that's indeed the case, err on the safe side.
        *refined = true;
      }

      // Also propagate handle shape and dtype of edges which are carrying
      // resource handles.
      if (ctx->input_types[dst_input] == DT_RESOURCE) {
        auto* outputs = src_ic->output_handle_shapes_and_types(src_output);
        if (!outputs) continue;
        auto* inputs = ic->input_handle_shapes_and_types(dst_input);

        if (!inputs || !EquivalentShapesAndTypes(*outputs, *inputs))
          *refined = true;
        ic->set_input_handle_shapes_and_types(dst_input, *outputs);
      }
    }

    // Make sure we schedule the fanout of resources (which have no input)
    // whenever the resources are updated.
    *refined |= ic->num_inputs() == 0;

    if (!*refined) {
      // No input shape has changed, we're done.
      return Status::OK();
    }

    // Convert all kUnknownDimFromConst to -1 for shape inference.
    ic->set_input_tensors_as_shapes(ReplaceUnknownDimFromConstWithUnknownDim(
        ic, ctx->input_tensors_as_shapes_to_propagate));
    // Note: UpdateFunction uses input_tensors_as_shapes and
    // input_tensor_protos (not the Tensor object) for input values.
    // so for function nodes, we don't need to convert TensorProtos
    // to Tensors here. If the current op is not a function op, we convert
    // TensorProtos to Tensors before calling InferShapes.

    // Properly handle function nodes.
    if (ctx->op_data && ctx->op_data->is_function_op) {
      // TODO(jmdecker): Detect if the input shapes have changed for this
      // function. Note that when we hit a function call node, refined will be
      // true, as the updates to the call node will have changed, even if it's
      // the same function being called twice with the same input shapes.
      // Example: simple_function.pbtxt
      if (aggressive_shape_inference_) {
        // If output shapes are annotated, use it and skip UpdateFunction();
        // it can be very expensive when a function node has nested function
        // nodes internally. One downside with this approach is that we do not
        // get output values or output shapes as tensor from function node.
        auto s = UpdateOutputShapesUsingAnnotatedInformation(*node, ctx);
        if (s.ok() && AllOutputShapesKnown(ctx)) {
          return Status::OK();
        }
        // If shape annotation was not available, incomplete, or incompatible,
        // fall through to call UpdateFunction().
      }
      auto s = UpdateFunction(node);
      if (s.ok()) {
        return Status::OK();
      } else {
        VLOG(1) << "UpdateFunction failed for " << node->op()
                << ". Defaulting to ShapeUnknown.\n"
                << s.ToString();
      }
    }

    //  Construct Tensors for constant inputs used by shape functions.
    std::vector<Tensor> const_values(ic->num_inputs());
    std::vector<const Tensor*> input_tensors(ic->num_inputs(), nullptr);
    for (int dst_input = 0; dst_input < ic->num_inputs(); ++dst_input) {
      const TensorProto* tensor_proto = ctx->input_tensor_protos[dst_input];
      if (tensor_proto != nullptr &&
          // Skip if the const tensor is too large.
          NumElementsFromTensorProto(*tensor_proto) <=
              kThresholdToSkipConstTensorInstantiation &&
          const_values[dst_input].FromProto(*tensor_proto)) {
        input_tensors[dst_input] = &const_values[dst_input];
      }
    }
    ic->set_input_tensors(input_tensors);

    // Update the shapes of the outputs.
    return InferShapes(*node, ctx);
  }