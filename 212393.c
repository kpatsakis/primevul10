void SetAttrValue(const Tensor& value, AttrValue* out) {
  if (value.NumElements() > 1) {
    value.AsProtoTensorContent(out->mutable_tensor());
  } else {
    value.AsProtoField(out->mutable_tensor());
  }
}