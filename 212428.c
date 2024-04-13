TfLiteRegistration* Register_DEPTHWISE_CONV_2D_UINT8() {
#ifdef USE_NEON
  return Register_DEPTHWISE_CONVOLUTION_NEON_OPT_UINT8();
#else
  return Register_DEPTHWISE_CONV_2D();
#endif
}