inline void SetActivationParams(int32_t min, int32_t max, P* params) {
  params->quantized_activation_min = min;
  params->quantized_activation_max = max;
}