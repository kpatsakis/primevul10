void DeAllocateUntrustedBuffers(void **free_list, size_t count) {
  if (!IsValidUntrustedAddress(free_list)) {
    TrustedPrimitives::BestEffortAbort(
        "free_list expected to be in untrusted memory.");
  }
  CHECK_OCALL(ocall_enc_untrusted_deallocate_free_list(
      free_list, static_cast<uint64_t>(count)));
}