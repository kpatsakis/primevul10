pid_t InvokeFork(const char *enclave_name, bool restore_snapshot) {
  int32_t ret;
  sgx_status_t status =
      ocall_enc_untrusted_fork(&ret, enclave_name, restore_snapshot);
  if (status != SGX_SUCCESS) {
    errno = EINTR;
    return -1;
  }
  return ret;
}