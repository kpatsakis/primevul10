TfLiteStatus Prepare(TfLiteContext* context, TfLiteNode* node) {
  OpData* data = reinterpret_cast<OpData*>(node->user_data);

  bool has_bias = NumInputs(node) == 4;

  // Sanity checks on op
  TF_LITE_ENSURE(context, has_bias || NumInputs(node) == 3);
  TF_LITE_ENSURE_EQ(context, NumOutputs(node), 1);

  // Retrieve tensors
  const TfLiteTensor* output_shape;
  TF_LITE_ENSURE_OK(
      context, GetInputSafe(context, node, kOutputShapeTensor, &output_shape));
  const TfLiteTensor* weights;
  TF_LITE_ENSURE_OK(context,
                    GetInputSafe(context, node, kWeightsTensor, &weights));
  const TfLiteTensor* input;
  TF_LITE_ENSURE_OK(context,
                    GetInputSafe(context, node, kDataInputTensor, &input));
  const TfLiteTensor* bias = nullptr;

  TfLiteTensor* output;
  TF_LITE_ENSURE_OK(context,
                    GetOutputSafe(context, node, kOutputTensor, &output));

  // Tensor sanity checks
  TF_LITE_ENSURE_EQ(context, NumDimensions(output_shape), 1);
  TF_LITE_ENSURE_EQ(context, NumDimensions(input), 4);
  TF_LITE_ENSURE_EQ(context, NumDimensions(weights), 4);
  TF_LITE_ENSURE(context,
                 input->type == kTfLiteFloat32 || input->type == kTfLiteUInt8 ||
                     input->type == kTfLiteInt8 || input->type == kTfLiteInt16);

  if (has_bias) {
    bias = GetOptionalInputTensor(context, node, kBiasTensor);
    if (bias) {
      if (input->type == kTfLiteUInt8 || input->type == kTfLiteInt8) {
        TF_LITE_ENSURE_TYPES_EQ(context, bias->type, kTfLiteInt32);
        if (input->type == kTfLiteInt8) {
          TF_LITE_ENSURE_EQ(context, bias->params.zero_point, 0);
        }
      } else if (input->type == kTfLiteInt16) {
        TF_LITE_ENSURE_EQ(context, bias->type, kTfLiteInt64);
        TF_LITE_ENSURE_EQ(context, bias->params.zero_point, 0);
      } else {
        TF_LITE_ENSURE_TYPES_EQ(context, bias->type, input->type);
      }
      TF_LITE_ENSURE_EQ(context, NumElements(bias),
                        SizeOfDimension(weights, 0));
    }
  }

  if (input->type == kTfLiteInt16) {
    TF_LITE_ENSURE_EQ(context, weights->type, kTfLiteInt8);
    TF_LITE_ENSURE_EQ(context, input->params.zero_point, 0);
    TF_LITE_ENSURE_EQ(context, output->params.zero_point, 0);
  } else {
    TF_LITE_ENSURE_TYPES_EQ(context, weights->type, input->type);
  }
  TF_LITE_ENSURE_TYPES_EQ(context, output->type, input->type);
  // Ensure that weights and inputs have the same channel dimension.
  // Note: TOCO will reorder weights in the following format: OHWI.
  TF_LITE_ENSURE_EQ(context, SizeOfDimension(input, 3),
                    SizeOfDimension(weights, 3));

  // Allocate col2Im, transposed_weights & scratch Tensor.
  TF_LITE_ENSURE_STATUS(AllocateTemporaryTensorsIfRequired<kernel_type>(
      context, input->type, weights->type, node));

  OpData* user_data = reinterpret_cast<OpData*>(node->user_data);
  TfLiteTensor* col2im = nullptr;
  if (data->has_col2im) {
    node->temporaries->data[data->col2im_index] = data->col2im_id;
    TF_LITE_ENSURE_OK(
        context,
        GetTemporarySafe(context, node, user_data->col2im_index, &col2im));
  }

  if (!IsConstantTensor(output_shape)) {
    // Defer resizing until Eval().
    SetTensorToDynamic(output);
    if (data->has_col2im) {
      SetTensorToDynamic(col2im);
    }
  } else {
    TF_LITE_ENSURE_STATUS(ResizeTensor(context, output_shape, output));
    if (data->has_col2im) {
      TF_LITE_ENSURE_STATUS(
          ResizeCol2ImTensor(context, output_shape, weights, input, col2im));
    }
  }

  if (data->weights_are_transposed) {
    node->temporaries->data[data->transposed_weights_index] =
        data->transposed_weights_id;
    TfLiteTensor* transposed_weights;
    TF_LITE_ENSURE_OK(
        context,
        GetTemporarySafe(context, node, user_data->transposed_weights_index,
                         &transposed_weights));
    if (!IsConstantTensor(weights)) {
      SetTensorToDynamic(transposed_weights);
    } else {
      ResizeAndTransposeWeights(context, weights, transposed_weights);
    }
  }

  if (input->type == kTfLiteUInt8 || input->type == kTfLiteInt8 ||
      input->type == kTfLiteInt16) {
    node->temporaries->data[data->scratch_tensor_index] =
        data->scratch_tensor_id;
    TfLiteTensor* scratch_buffer;
    TF_LITE_ENSURE_OK(
        context, GetTemporarySafe(context, node, data->scratch_tensor_index,
                                  &scratch_buffer));
    if (input->type == kTfLiteInt16) {
      scratch_buffer->type = kTfLiteInt64;
    } else {
      scratch_buffer->type = kTfLiteInt32;
    }

    scratch_buffer->allocation_type = kTfLiteDynamic;
    if (!IsConstantTensor(output_shape)) {
      SetTensorToDynamic(scratch_buffer);
    } else {
      TF_LITE_ENSURE_STATUS(
          ResizeTensor(context, output_shape, scratch_buffer));
    }

    TF_LITE_ENSURE_EQ(context, weights->quantization.type,
                      kTfLiteAffineQuantization);
    const auto* affine_quantization =
        reinterpret_cast<TfLiteAffineQuantization*>(
            weights->quantization.params);
    const int channels_out = weights->dims->data[0];
    TF_LITE_ENSURE(context, affine_quantization);
    TF_LITE_ENSURE(context, affine_quantization->scale);
    TF_LITE_ENSURE(context, (affine_quantization->scale->size == 1 ||
                             affine_quantization->scale->size == channels_out));

    data->per_channel_output_multiplier.resize(channels_out);
    data->per_channel_output_shift.resize(channels_out);
    TF_LITE_ENSURE_STATUS(tflite::PopulateConvolutionQuantizationParams(
        context, input, weights, bias, output, kTfLiteActNone,
        &data->output_multiplier, &data->output_shift,
        &data->output_activation_min, &data->output_activation_max,
        data->per_channel_output_multiplier.data(),
        data->per_channel_output_shift.data(), channels_out));
  }

  return kTfLiteOk;
}