void SetAttrValue(const gtl::ArraySlice<Tensor> value, AttrValue* out) {
  out->mutable_list()->Clear();  // Create list() even if value empty.
  for (const auto& v : value) {
    if (v.NumElements() > 1) {
      v.AsProtoTensorContent(out->mutable_list()->add_tensor());
    } else {
      v.AsProtoField(out->mutable_list()->add_tensor());
    }
  }
}