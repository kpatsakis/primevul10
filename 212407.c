uint64 TensorProtoHash(const TensorProto& tp) {
  Tensor tensor(tp.dtype());
  bool success = tensor.FromProto(tp);
  DCHECK(success);
  TensorProto p;
  tensor.AsProtoTensorContent(&p);
  return DeterministicProtoHash64(p);
}