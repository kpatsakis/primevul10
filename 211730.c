  static void compileTimeAssertions() {
    static_assert(offsetof(StrNR, m_px) == kExpectedMPxOffset, "");
  }