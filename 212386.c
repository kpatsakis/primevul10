uint64 FastTensorProtoHash(const TensorProto& tp) {
  if (TensorByteSize(tp) > kMaxAttrValueTensorByteSize) {
    return DeterministicProtoHash64(tp);
  } else {
    return TensorProtoHash(tp);
  }
}