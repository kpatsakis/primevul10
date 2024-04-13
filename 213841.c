  void Compute(OpKernelContext* ctx) override {
    const Tensor* input_tensor;
    OP_REQUIRES_OK(ctx, ctx->input("input", &input_tensor));

    // Go through all the strings in `input`.
    const auto& input_vec = input_tensor->flat<tstring>();

    std::unique_ptr<WrappedConverter> input_encoder =
        absl::make_unique<WrappedConverter>();
    input_encoder->init(input_encoding_);
    OP_REQUIRES(ctx, input_encoder->converter_,
                errors::InvalidArgument(
                    "Could not create converter for input encoding: " +
                    input_encoding_));

    std::vector<UChar32> char_values;
    std::vector<SPLITS_TYPE> offset_values;

    Tensor* output_row_splits;
    OP_REQUIRES_OK(ctx, ctx->allocate_output("row_splits",
                                             {input_tensor->NumElements() + 1},
                                             &output_row_splits));
    auto out_row_splits = output_row_splits->vec<SPLITS_TYPE>();

    int row_split_index = 0;
    SPLITS_TYPE next_row_split = 0;
    for (int i = 0; i < input_vec.size(); ++i) {
      const string& input = input_vec(i);
      // Convert input strings into unicode values. Output to a list of
      // char_values, record row splits and char_to_byte_starts, which are all
      // the fields needed to construct a RaggedTensor.
      out_row_splits(row_split_index) = next_row_split;
      row_split_index++;
      int current_offset = 0;
      IterateUnicodeString(
          input, input_encoder->converter_,
          std::bind(&UnicodeDecodeBaseOp::Decode, this, ctx, &char_values,
                    &offset_values, &current_offset, &next_row_split,
                    std::placeholders::_1, std::placeholders::_2,
                    std::placeholders::_3));
    }
    out_row_splits(row_split_index) = next_row_split;

    Tensor* output_char_values;
    OP_REQUIRES_OK(
        ctx, ctx->allocate_output(
                 "char_values", {static_cast<SPLITS_TYPE>(char_values.size())},
                 &output_char_values));
    auto out_char_values = output_char_values->vec<int32>();
    if (generate_offsets_) {
      DCHECK(offset_values.size() == char_values.size());
      Tensor* output_offset_values;
      OP_REQUIRES_OK(ctx, ctx->allocate_output(
                              "char_to_byte_starts",
                              {static_cast<SPLITS_TYPE>(offset_values.size())},
                              &output_offset_values));
      auto out_offset_values = output_offset_values->vec<SPLITS_TYPE>();

      // Load output tensors from intermediate value arrays.
      for (int i = 0; i < char_values.size(); ++i) {
        out_char_values(i) = static_cast<int32>(char_values[i]);
        out_offset_values(i) = offset_values[i];
      }
    } else {
      for (int i = 0; i < char_values.size(); ++i) {
        out_char_values(i) = static_cast<int32>(char_values[i]);
      }
    }
  }