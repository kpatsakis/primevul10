  ~FreeHelperImpl() {
    useSmartFree ? req::free(p) : free(p);
  }