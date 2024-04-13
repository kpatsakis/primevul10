UntrustedCacheMalloc *UntrustedCacheMalloc::Instance() {
  static TrustedSpinLock lock(/*is_recursive=*/false);
  static UntrustedCacheMalloc *instance = nullptr;
  if (instance == nullptr) {
    LockGuard guard(&lock);
    if (instance == nullptr) {
      instance = new UntrustedCacheMalloc();
    }
  }
  return instance;
}