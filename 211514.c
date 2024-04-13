  TypedValue asTypedValue() const {
    assertx(m_str);
    return make_tv<KindOfString>(m_str.get());
  }