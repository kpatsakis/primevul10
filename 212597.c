  bool parseRue() {
    if (*p++ != 'r') return false;
    if (*p++ != 'u') return false;
    if (*p++ != 'e') return false;
    auto const tv = top++;
    tv->m_type = KindOfBoolean;
    tv->m_data.num = true;
    return true;
  }