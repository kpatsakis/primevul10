void *TrustedPrimitives::UntrustedLocalAlloc(size_t size) noexcept {
  void *result;
  CHECK_OCALL(
      ocall_untrusted_local_alloc(&result, static_cast<uint64_t>(size)));
  if (result && !IsOutsideEnclave(result, static_cast<uint64_t>(size))) {
    TrustedPrimitives::BestEffortAbort(
        "Allocated memory not found to be outside the enclave.");
  }

  // On error, malloc returns nullptr and sets errno to ENOMEM.
  if (!result) {
    errno = ENOMEM;
    TrustedPrimitives::DebugPuts("UntrustedLocalAlloc on SGX failed.");
  }
  return result;
}