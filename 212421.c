TfLiteRegistration* Register_DEPTHWISE_CONVOLUTION_GENERIC_OPT() {
  static TfLiteRegistration r = {
      depthwise_conv::Init, depthwise_conv::Free, depthwise_conv::Prepare,
      depthwise_conv::Eval<depthwise_conv::kGenericOptimized>};
  return &r;
}