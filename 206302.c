  static void H2Free(void* ptr, void* user_data) {
    if (ptr == nullptr) return;  // free(null); happens quite often.
    void* result = H2Realloc(ptr, 0, user_data);
    CHECK_EQ(result, nullptr);
  }