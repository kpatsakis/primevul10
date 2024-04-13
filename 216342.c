inline void GetActivationParams(const P& params, int64_t* min, int64_t* max) {
  *min = params.int64_activation_min;
  *max = params.int64_activation_max;
}