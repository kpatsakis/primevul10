void TrustedPrimitives::DebugPuts(const char *message) {
  int result;
  CHECK_OCALL(ocall_untrusted_debug_puts(&result, message));
  if (result < 0) {
    errno = EOF;
  }
}