  void pushStringData(StringData* data) {
    auto const tv = top++;
    tv->m_type = KindOfString;
    tv->m_data.pstr = data;
  }