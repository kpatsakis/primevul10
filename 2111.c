  static bool NeedsEscaping(const string& s) {
    for (auto c : s) {
      if (!isalnum(c) && (c != ' ')) {
        return true;
      }
    }
    return false;
  }