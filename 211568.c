  void checkArcBuff(int len) {
    len = 2*len + HP_STACK_DELIM_LEN + 2;
    if (len >= m_arcBuffLen) {
      m_arcBuffLen *= 2;
      m_arcBuff = (char *)realloc(m_arcBuff, m_arcBuffLen);
      if (m_arcBuff == nullptr) {
        throw std::bad_alloc();
      }
    }
  }