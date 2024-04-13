inline void GetActivationParams(const P& params, int32_t* min, int32_t* max) {
  *min = params.quantized_activation_min;
  *max = params.quantized_activation_max;
}