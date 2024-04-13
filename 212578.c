  static void json_free(void* ptr) {
    if (RuntimeOption::EvalJsonParserUseLocalArena) {
      return local_free(ptr);
    } else {
      return free(ptr);
    }
  }