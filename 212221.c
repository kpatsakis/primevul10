inline String& String::operator=(const StaticString& v) {
  m_str = req::ptr<StringData>::attach(v.m_str.get());
  return *this;
}