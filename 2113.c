NodeDef FunctionDefHelper::Node::ToNodeDef() const {
  NodeDef n;
  n.set_op(this->op);
  n.set_name(GetName());
  for (const auto& a : this->attr) {
    n.mutable_attr()->insert({a.first, a.second.proto});
  }
  for (const string& a : this->arg) {
    n.add_input(a);
  }
  for (const string& d : this->dep) {
    n.add_input(strings::StrCat("^", d));
  }
  if (!this->device.empty()) {
    n.set_device(this->device);
  }
  if (!this->original_node_names.empty()) {
    *n.mutable_experimental_debug_info()->mutable_original_node_names() = {
        this->original_node_names.begin(), this->original_node_names.end()};
  }
  if (!this->original_func_names.empty()) {
    *n.mutable_experimental_debug_info()->mutable_original_func_names() = {
        this->original_func_names.begin(), this->original_func_names.end()};
  }
  return n;
}