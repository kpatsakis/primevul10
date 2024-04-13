bool AreTensorProtosEqual(const TensorProto& lhs, const TensorProto& rhs,
                          bool allow_false_negatives) {
  // A small TensorProto can expand into a giant Tensor.  So we avoid
  // conversion to an actual Tensor if we can quickly rule out equality
  // by comparing the Tensor size since different sized Tensors are definitely
  // different.
  const int64_t lhs_tensor_bytes = TensorByteSize(lhs);
  const int64_t rhs_tensor_bytes = TensorByteSize(rhs);
  if (lhs_tensor_bytes != rhs_tensor_bytes) {
    return false;
  }

  // If the TensorProto representation expands into a much bigger Tensor,
  // we have a fast-path that first compares the protos.
  const int64_t lhs_proto_bytes = lhs.ByteSizeLong();
  const bool large_expansion =
      (lhs_proto_bytes < 512 && lhs_tensor_bytes > 4096);

  // If the tensor is very large, we'll only compare the proto representation if
  // false negatives are allowed. This may miss some equivalent tensors whose
  // actual tensor values are the same but which are described by different
  // TensorProtos. This avoids construction of large protos in memory.
  const bool only_compare_proto =
      (allow_false_negatives && lhs_tensor_bytes > kMaxAttrValueTensorByteSize);
  if (large_expansion || only_compare_proto) {
    if (AreSerializedProtosEqual(lhs, rhs))
      return true;
    else if (only_compare_proto)
      return false;
  }

  // Finally, compare them by constructing Tensors and serializing them back.
  // There are multiple equivalent representations of attr values containing
  // TensorProtos. Comparing Tensor objects is pretty tricky. This is unsafe
  // operation, because large tensors can be represented as TensorProto, but
  // can't be serialized to tensor content.
  Tensor lhs_t(lhs.dtype());
  bool success = lhs_t.FromProto(lhs);
  if (!success) {
    return false;
  }

  Tensor rhs_t(rhs.dtype());
  success = rhs_t.FromProto(rhs);
  if (!success) {
    return false;
  }

  TensorProto lhs_tp;
  lhs_t.AsProtoTensorContent(&lhs_tp);

  TensorProto rhs_tp;
  rhs_t.AsProtoTensorContent(&rhs_tp);

  return AreSerializedProtosEqual(lhs_tp, rhs_tp);
}