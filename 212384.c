void SetAttrValue(StringPiece value, AttrValue* out) {
  out->set_s(value.data(), value.size());
}