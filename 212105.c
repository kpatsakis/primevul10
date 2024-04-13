inline const StringData::Proxy* StringData::proxy() const {
  return static_cast<const Proxy*>(payload());
}