void **AllocateUntrustedBuffers(size_t count, size_t size) {
  void **buffers;
  CHECK_OCALL(ocall_enc_untrusted_allocate_buffers(
      &buffers, static_cast<uint64_t>(count), static_cast<uint64_t>(size)));
  if (!buffers || !TrustedPrimitives::IsOutsideEnclave(buffers, size)) {
    TrustedPrimitives::BestEffortAbort(
        "allocated buffers (for use by UntrustedCacheMalloc) found to not be "
        "in untrusted memory.");
  }
  return buffers;
}