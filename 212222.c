inline String::String(const StaticString& str) :
  m_str(str.m_str.get(), NoIncRef{}) {
  assertx(str.m_str->isStatic());
}