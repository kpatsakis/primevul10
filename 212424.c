TfLiteRegistration* Register_DEPTHWISE_CONVOLUTION_REF() {
  static TfLiteRegistration r = {
      depthwise_conv::Init, depthwise_conv::Free, depthwise_conv::Prepare,
      depthwise_conv::Eval<depthwise_conv::kReference>};
  return &r;
}