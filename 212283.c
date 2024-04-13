TfLiteStatus EvalQuantized(TfLiteContext* context, TfLiteNode* node,
                           TfLiteFullyConnectedParams* params, OpData* data,
                           const TfLiteTensor* input,
                           const TfLiteTensor* filter, const TfLiteTensor* bias,
                           TfLiteTensor* output) {
  int32_t input_offset = -input->params.zero_point;
  int32_t filter_offset = -filter->params.zero_point;
  int32_t output_offset = output->params.zero_point;
  // Only the Pie path supports quantized models and float inputs/outputs.
  if (input->type == kTfLiteFloat32) {
    TfLiteTensor* input_quantized;
    TF_LITE_ENSURE_OK(context, GetTemporarySafe(context, node, /*index=*/0,
                                                &input_quantized));
    TfLiteTensor* scaling_factors;
    TF_LITE_ENSURE_OK(context, GetTemporarySafe(context, node, /*index=*/1,
                                                &scaling_factors));
    TfLiteTensor* accum_scratch;
    TF_LITE_ENSURE_OK(
        context, GetTemporarySafe(context, node, /*index=*/2, &accum_scratch));
    TfLiteTensor* input_offsets;
    TF_LITE_ENSURE_OK(
        context, GetTemporarySafe(context, node, /*index=*/3, &input_offsets));
    TfLiteTensor* row_sums;
    TF_LITE_ENSURE_OK(context,
                      GetTemporarySafe(context, node, /*index=*/4, &row_sums));
    return EvalHybrid(context, node, params, data, input, filter, bias,
                      input_quantized, scaling_factors, accum_scratch, row_sums,
                      input_offsets, output);
  } else {
    FullyConnectedParams op_params;
    op_params.input_offset = input_offset;
    op_params.weights_offset = filter_offset;
    op_params.output_offset = output_offset;
    op_params.output_multiplier = data->output_multiplier;
    op_params.output_shift = data->output_shift;
    op_params.quantized_activation_min = data->output_activation_min;
    op_params.quantized_activation_max = data->output_activation_max;
    op_params.lhs_cacheable = IsConstantTensor(filter);
    op_params.rhs_cacheable = IsConstantTensor(input);
    switch (output->type) {
      case kTfLiteUInt8:
        if (kernel_type == kReference) {
          reference_ops::FullyConnected(
              op_params, GetTensorShape(input), GetTensorData<uint8_t>(input),
              GetTensorShape(filter), GetTensorData<uint8_t>(filter),
              GetTensorShape(bias), GetTensorData<int32_t>(bias),
              GetTensorShape(output), GetTensorData<uint8_t>(output));
        } else {
          optimized_ops::FullyConnected(
              op_params, GetTensorShape(input), GetTensorData<uint8_t>(input),
              GetTensorShape(filter), GetTensorData<uint8_t>(filter),
              GetTensorShape(bias), GetTensorData<int32_t>(bias),
              GetTensorShape(output), GetTensorData<uint8_t>(output),
              CpuBackendContext::GetFromContext(context));
        }
        break;
      case kTfLiteInt8:
        FullyConnectedInt8<kernel_type>(
            data, input, filter, bias, output,
            CpuBackendContext::GetFromContext(context));
        break;
      case kTfLiteInt16:
        if (input->type == kTfLiteInt16) {
          // To avoid 32bit accum overflow, it enables RUY only
          // when zero_point is 0.
          bool has_non_zero_point = input->params.zero_point ||
                                    filter->params.zero_point ||
                                    output->params.zero_point;
          if (kernel_type == kReference || has_non_zero_point) {
            FullyConnectedInt16<kernel_type>(data, input, filter, bias, output);
          } else {
            // Currently, Ruy cannot support int64_t bias. Before Ruy supports
            // it, it adds bias to Ruy gemm result without bias.
            optimized_integer_ops::FullyConnected(
                op_params, GetTensorShape(input), GetTensorData<int16_t>(input),
                GetTensorShape(filter), GetTensorData<int8_t>(filter),
                RuntimeShape(), nullptr, GetTensorShape(output),
                GetTensorData<int16_t>(output),
                CpuBackendContext::GetFromContext(context));
            if (bias) {
              reference_ops::AddBiasToOutput(
                  op_params, GetTensorData<int64_t>(bias),
                  GetTensorShape(output), GetTensorData<int16_t>(output));
            }
          }
        } else if (kernel_type == kReference) {
          reference_ops::FullyConnected(
              op_params, GetTensorShape(input), GetTensorData<uint8_t>(input),
              GetTensorShape(filter), GetTensorData<uint8_t>(filter),
              GetTensorShape(bias), GetTensorData<int32_t>(bias),
              GetTensorShape(output), GetTensorData<int16_t>(output));
        } else {
          optimized_ops::FullyConnected(
              op_params, GetTensorShape(input), GetTensorData<uint8_t>(input),
              GetTensorShape(filter), GetTensorData<uint8_t>(filter),
              GetTensorShape(bias), GetTensorData<int32_t>(bias),
              GetTensorShape(output), GetTensorData<int16_t>(output),
              CpuBackendContext::GetFromContext(context));
        }
        break;
      default:
        context->ReportError(context,
                             "Quantized FullyConnected expects output data "
                             "type uint8, int8 or int16");
        return kTfLiteError;
    }
  }

  return kTfLiteOk;
}