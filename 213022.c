bool TrustedPrimitives::IsInsideEnclave(const void *addr, size_t size) {
  return sgx_is_within_enclave(addr, size) == 1;
}