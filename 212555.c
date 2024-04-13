  void pushDouble(double data) {
    auto const tv = top++;
    tv->m_type = KindOfDouble;
    tv->m_data.dbl = data;
  }