inline bool NextIndex(const int num_dims, const int* dims, int* current) {
  if (num_dims == 0) {
    return false;
  }
  TFLITE_DCHECK(dims != nullptr);
  TFLITE_DCHECK(current != nullptr);
  int carry = 1;
  for (int idx = num_dims - 1; idx >= 0; --idx) {
    int current_val = current[idx] + carry;
    TFLITE_DCHECK_GE(dims[idx], current_val);
    if (dims[idx] == current_val) {
      current[idx] = 0;
    } else {
      current[idx] = current_val;
      carry = 0;
      break;
    }
  }
  return (carry == 0);
}