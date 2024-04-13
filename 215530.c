int ecall_take_snapshot(char **output, uint64_t *output_len) {
  int result = 0;
  size_t tmp_output_len;
  try {
    result = asylo::TakeSnapshot(output, &tmp_output_len);
  } catch (...) {
    LOG(FATAL) << "Uncaught exception in enclave";
  }

  if (output_len) {
    *output_len = static_cast<uint64_t>(tmp_output_len);
  }
  return result;
}