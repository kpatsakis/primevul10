uint32_t enc_untrusted_ql_set_quote_config(const sgx_ql_config_t *config) {
  uint32_t result;
  CHECK_OCALL(ocall_enc_untrusted_ql_set_quote_config(
      &result, config, config->cert_data_size, config->p_cert_data));
  return result;
}