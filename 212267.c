  static void compileTimeAssertions() {
    static_assert(sizeof(String) == sizeof(req::ptr<StringData>), "");
  }