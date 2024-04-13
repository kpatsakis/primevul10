TfLiteRegistration* Register_DEPTHWISE_CONV_2D() {
#ifdef USE_NEON
  return Register_DEPTHWISE_CONVOLUTION_NEON_OPT();
#else
  return Register_DEPTHWISE_CONVOLUTION_GENERIC_OPT();
#endif
}