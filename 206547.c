  void PushPath(std::string&& path) {
    paths.emplace_back(std::move(path));
  }