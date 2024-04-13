uint32_t enc_untrusted_qe_get_quote(const sgx_report_t *app_report,
                                    uint32_t quote_size, uint8_t *quote) {
  uint32_t result;
  CHECK_OCALL(
      ocall_enc_untrusted_qe_get_quote(&result, app_report, quote_size, quote));
  return result;
}