void SetAttrValue(const NameAttrList& value, AttrValue* out) {
  *out->mutable_func() = value;
}