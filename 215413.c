void *untrusted_cache_malloc(size_t size) {
  asylo::UntrustedCacheMalloc *instance =
      asylo::UntrustedCacheMalloc::Instance();
  return instance->Malloc(size);
}