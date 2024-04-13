void SetAttrValue(gtl::ArraySlice<tstring> value, AttrValue* out) {
  out->mutable_list()->Clear();
  for (const auto& v : value) {
    out->mutable_list()->add_s(v.data(), v.size());
  }
}