inline StringData::Proxy* StringData::proxy() {
  return static_cast<Proxy*>(payload());
}