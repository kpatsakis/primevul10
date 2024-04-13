void SetAttrValue(const tstring& value, AttrValue* out) {
  out->set_s(value.data(), value.size());
}