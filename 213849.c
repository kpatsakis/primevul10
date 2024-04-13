  void Compute(OpKernelContext* ctx) override {
    const Tensor* input_tensor;
    OP_REQUIRES_OK(ctx, ctx->input("input", &input_tensor));

    static thread_local std::unique_ptr<WrappedConverter> input_encoder;
    if (!input_encoder) {
      input_encoder.reset(new WrappedConverter());
    }
    input_encoder->init(input_encoding_);
    OP_REQUIRES(ctx, input_encoder->converter_,
                errors::InvalidArgument(
                    "Could not create converter for input encoding: " +
                    input_encoding_));

    // Output may be forwardable from input, in which case work in-place.
    Tensor* output_tensor;
    std::unique_ptr<Tensor> maybe_forwarded =
        ctx->forward_input(0 /*input_index*/, 0 /*output_index*/,
                           tensorflow::DT_STRING, input_tensor->shape(),
                           ctx->input_memory_type(0), ctx->input_alloc_attr(0));
    if (maybe_forwarded) {
      output_tensor = maybe_forwarded.get();
      OP_REQUIRES_OK(ctx, ctx->set_output("output", *output_tensor));
    } else {
      OP_REQUIRES_OK(ctx, ctx->allocate_output("output", input_tensor->shape(),
                                               &output_tensor));
      output_tensor->flat<tstring>() = input_tensor->flat<tstring>();
    }

    auto output_flat = output_tensor->flat<tstring>();
    bool found_any_format_error = false;
    for (size_t i = 0; i < output_flat.size(); ++i) {
      Transcode(&(output_flat(i)), input_encoder->converter_,
                &found_any_format_error);
    }
    if (error_options_.error_on_malformatting && found_any_format_error) {
      ctx->CtxFailure(
          errors::InvalidArgument("Invalid formatting on input string"));
    }
  }