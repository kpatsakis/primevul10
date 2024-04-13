  ~StrNR() {
    if (debug) {
      m_px = reinterpret_cast<StringData*>(0xdeadbeeffaceb004);
    }
  }