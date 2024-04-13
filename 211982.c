  static bool write_rand_file(const char *file, int egdsocket, int seeded) {
    char buffer[PATH_MAX];

    if (egdsocket || !seeded) {
      /* if we did not manage to read the seed file, we should not write
       * a low-entropy seed file back */
      return false;
    }
    if (file == nullptr) {
      file = RAND_file_name(buffer, sizeof(buffer));
    }
    if (file == nullptr || !RAND_write_file(file)) {
      raise_warning("unable to write random state");
      return false;
    }
    return true;
  }