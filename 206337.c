  static void* H2Calloc(size_t nmemb, size_t size, void* user_data) {
    size_t real_size = MultiplyWithOverflowCheck(nmemb, size);
    void* mem = H2Malloc(real_size, user_data);
    if (mem != nullptr)
      memset(mem, 0, real_size);
    return mem;
  }