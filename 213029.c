void *TrustedPrimitives::UntrustedLocalMemcpy(void *dest, const void *src,
                                              size_t size) noexcept {
  return memcpy(dest, src, size);
}