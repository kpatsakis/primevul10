bool TrustedPrimitives::IsOutsideEnclave(const void *addr, size_t size) {
  return sgx_is_outside_enclave(addr, size) == 1;
}