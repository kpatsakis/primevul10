FunctionDef FunctionDefHelper::Create(
    const string& function_name, gtl::ArraySlice<string> in_def,
    gtl::ArraySlice<string> out_def, gtl::ArraySlice<string> attr_def,
    gtl::ArraySlice<Node> node_def,
    gtl::ArraySlice<std::pair<string, string>> ret_def,
    gtl::ArraySlice<std::pair<string, string>> control_ret_def) {
  FunctionDef fdef;

  // Signature
  OpDefBuilder b(function_name);
  for (const auto& i : in_def) b.Input(i);
  for (const auto& o : out_def) b.Output(o);
  for (const auto& a : attr_def) b.Attr(a);
  for (const auto& c : control_ret_def) b.ControlOutput(c.first);

  OpRegistrationData op_reg_data;
  TF_CHECK_OK(b.Finalize(&op_reg_data));
  fdef.mutable_signature()->Swap(&op_reg_data.op_def);

  // Function body
  for (const auto& n : node_def) {
    *(fdef.add_node_def()) = n.ToNodeDef();
  }

  // Returns
  for (const auto& r : ret_def) {
    fdef.mutable_ret()->insert({r.first, r.second});
  }

  // Control returns
  for (const auto& cr : control_ret_def) {
    fdef.mutable_control_ret()->insert({cr.first, cr.second});
  }

  auto* op_def_registry = OpRegistry::Global();
  // Check if any op is stateful.
  for (const auto& n : node_def) {
    const OpDef* op_def = nullptr;
    auto status = op_def_registry->LookUpOpDef(n.op, &op_def);
    // Lookup can fail if e.g. we are calling a function that was not yet
    // defined.  If it happens, conservatively assume the op is stateful.
    if (!status.ok() || op_def->is_stateful()) {
      fdef.mutable_signature()->set_is_stateful(true);
    }
  }

  return fdef;
}