  void pushInt64(int64_t data) {
    auto const tv = top++;
    tv->m_type = KindOfInt64;
    tv->m_data.num = data;
  }