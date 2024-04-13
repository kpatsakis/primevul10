  static void* H2Malloc(size_t size, void* user_data) {
    return H2Realloc(nullptr, size, user_data);
  }