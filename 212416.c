uint64 AttrValueHash(const AttrValue& a) {
  return AttrValueHash(a, TensorProtoHash);
}