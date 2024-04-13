uint64 AttrValueHash(const AttrValue& a, const TensorProtoHasher& tensor_hash) {
  if (a.has_tensor()) return tensor_hash(a.tensor());

  if (a.has_func()) {
    const NameAttrList& func = a.func();
    uint64 h = Hash64(func.name());
    std::map<string, AttrValue> map(func.attr().begin(), func.attr().end());
    for (const auto& pair : map) {
      h = Hash64(pair.first.data(), pair.first.size(), h);
      h = Hash64Combine(AttrValueHash(pair.second, tensor_hash), h);
    }
    return h;
  }

  // If `a` is not a tensor or func, get a hash of serialized string.
  return DeterministicProtoHash64(a);
}