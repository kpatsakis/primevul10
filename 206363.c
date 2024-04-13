  std::string PopPath() {
    CHECK_GT(paths.size(), 0);
    std::string path = std::move(paths.back());
    paths.pop_back();
    return path;
  }