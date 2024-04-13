string SummarizeTensor(const TensorProto& tensor_proto) {
  Tensor t;
  if (!t.FromProto(tensor_proto)) {
    return strings::StrCat(
        "<Invalid TensorProto: ", tensor_proto.ShortDebugString(), ">");
  }
  return t.DebugString();
}