uint64 FastAttrValueHash(const AttrValue& a) {
  return AttrValueHash(a, FastTensorProtoHash);
}