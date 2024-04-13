  static void* json_malloc(size_t size) {
    if (RuntimeOption::EvalJsonParserUseLocalArena) {
      return local_malloc(size);
    } else {
      return malloc(size);
    }
  }