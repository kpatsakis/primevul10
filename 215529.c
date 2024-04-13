int ecall_transfer_secure_snapshot_key(const char *input, uint64_t input_len,
                                       char **output, uint64_t *output_len) {
  int result = 0;
  uint64_t bridge_output_len;
  try {
    result = asylo::TransferSecureSnapshotKey(
        input, static_cast<size_t>(input_len), output, &bridge_output_len);
  } catch (...) {
    LOG(FATAL) << "Uncaught exception in enclave";
  }
  if (output_len) {
    *output_len = static_cast<size_t>(bridge_output_len);
  }
  return result;
}