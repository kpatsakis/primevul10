uint32_t enc_untrusted_qe_get_quote_size(uint32_t *quote_size) {
  uint32_t result;
  CHECK_OCALL(ocall_enc_untrusted_qe_get_quote_size(&result, quote_size));
  return result;
}