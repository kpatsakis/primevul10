inline StringData* StringData::modifyChar(int offset, char c) {
  assertx(offset >= 0 && offset < size());
  assertx(!hasMultipleRefs());

  auto const sd = isProxy() ? escalate(size()) : this;
  sd->mutableData()[offset] = c;
  sd->m_hash = 0;
  return sd;
}