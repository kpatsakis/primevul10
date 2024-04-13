void untrusted_cache_free(void *buffer) {
  asylo::UntrustedCacheMalloc *instance =
      asylo::UntrustedCacheMalloc::Instance();
  instance->Free(buffer);
}