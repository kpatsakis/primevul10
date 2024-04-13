uint32_t enc_untrusted_qe_get_target_info(sgx_target_info_t *qe_target_info) {
  uint32_t result;
  CHECK_OCALL(ocall_enc_untrusted_qe_get_target_info(&result, qe_target_info));
  return result;
}