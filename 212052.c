  const String& shrink(size_t len) {
    assertx(m_str && !m_str->isImmutable());
    if (m_str->capacity() - len > kMinShrinkThreshold) {
      m_str = req::ptr<StringData>::attach(m_str->shrinkImpl(len));
    } else {
      assertx(len < StringData::MaxSize);
      m_str->setSize(len);
    }
    return *this;
  }