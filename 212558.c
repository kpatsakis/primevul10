  bool parseAlse() {
    if (*p++ != 'a') return false;
    if (*p++ != 'l') return false;
    if (*p++ != 's') return false;
    if (*p++ != 'e') return false;
    auto const tv = top++;
    tv->m_type = KindOfBoolean;
    tv->m_data.num = false;
    return true;
  }