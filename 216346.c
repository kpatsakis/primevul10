inline void GetActivationParams(const P& params, float* min, float* max) {
  *min = params.float_activation_min;
  *max = params.float_activation_max;
}