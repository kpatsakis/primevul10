void TrustedPrimitives::UntrustedLocalFree(void *ptr) noexcept {
  CHECK_OCALL(ocall_untrusted_local_free(ptr));
}