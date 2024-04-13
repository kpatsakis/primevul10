void TrustedPrimitives::BestEffortAbort(const char *message) {
  DebugPuts(message);
  delete UntrustedCacheMalloc::Instance();
  enc_reject_entries();
  MarkEnclaveAborted();
  abort();
}