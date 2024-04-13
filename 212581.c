  bool parseUll() {
    if (*p++ != 'u') return false;
    if (*p++ != 'l') return false;
    if (*p++ != 'l') return false;
    top++->m_type = KindOfNull;
    return true;
  }