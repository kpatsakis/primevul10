Status InstantiateFunction(const FunctionDef& fdef, AttrSlice attr_values,
                           GetFunctionSignature get_function,
                           InstantiationResult* result) {
  if (VLOG_IS_ON(5)) {
    const auto& signature = fdef.signature();
    VLOG(5) << "Instantiate function definition: name=" << signature.name()
            << " #input_args=" << signature.input_arg_size()
            << " #output_args=" << signature.output_arg_size()
            << " #control_output=" << signature.control_output_size();
    for (const auto& line : str_util::Split(Print(fdef), '\n')) {
      VLOG(5) << "|| " << line;
    }
  }

  const OpDef& sig = fdef.signature();
  TF_RETURN_IF_ERROR(ValidateSignatureWithAttrs(sig, attr_values));

  bool ints_on_device =
      fdef.attr().count(FunctionLibraryDefinition::kIntsOnDeviceAttr) != 0 &&
      fdef.attr().at(FunctionLibraryDefinition::kIntsOnDeviceAttr).b();

  FunctionInstantiationHelper helper(get_function, result);
  Status s;
  for (int i = 0, e = sig.input_arg_size(); i < e; ++i) {
    const OpDef::ArgDef& arg_def = sig.input_arg(i);
    auto it = fdef.arg_attr().find(i);
    const FunctionDef::ArgAttrs* arg_attrs =
        it != fdef.arg_attr().end() ? &it->second : nullptr;
    auto resource_id_it = fdef.resource_arg_unique_id().find(i);
    int64_t resource_arg_unique_id =
        resource_id_it != fdef.resource_arg_unique_id().end()
            ? resource_id_it->second
            : -1LL;
    s = helper.BuildInputArgIndex(arg_def, attr_values, arg_attrs,
                                  ints_on_device, resource_arg_unique_id);

    if (!s.ok()) {
      errors::AppendToMessage(&s, "In ", Print(arg_def));
      return s;
    }
  }

  auto substitute = [attr_values, &sig](StringPiece name, AttrValue* val) {
    // Look for a specified value...
    if (const AttrValue* v = attr_values.Find(name)) {
      *val = *v;
      return true;
    }
    // .. and if not, then check for a default value.
    if (const OpDef::AttrDef* attr = FindAttr(name, sig)) {
      if (attr->has_default_value()) {
        *val = attr->default_value();
        return true;
      }
    }
    // No luck finding a substitution.
    return false;
  };

  // Makes a copy of all attrs in fdef and substitutes placeholders.
  // After this step, every attr is bound to a concrete value.
  std::vector<AttrValueMap> node_attrs;
  node_attrs.resize(fdef.node_def_size());
  for (int i = 0; i < fdef.node_def_size(); ++i) {
    for (auto attr : fdef.node_def(i).attr()) {
      if (!SubstitutePlaceholders(substitute, &attr.second)) {
        return errors::InvalidArgument("Failed to bind all placeholders in ",
                                       SummarizeAttrValue(attr.second));
      }
      if (!node_attrs[i].insert(attr).second) {
        return errors::Internal("Somehow duplicated: ", attr.first);
      }
    }
    TF_RETURN_IF_ERROR(
        AddDefaultAttrs(fdef.node_def(i).op(), get_function, &node_attrs[i]));
  }

  for (int i = 0; i < fdef.node_def_size(); ++i) {
    s = helper.BuildNodeOutputIndex(fdef.node_def(i), AttrSlice(&node_attrs[i]),
                                    result->nodes.size() + i);
    if (!s.ok()) {
      errors::AppendToMessage(&s, "In ",
                              FormatNodeDefForError(fdef.node_def(i)));
      return s;
    }
  }
  // Emits one node for each fdef.node_def.
  for (int i = 0; i < fdef.node_def_size(); ++i) {
    s = helper.InstantiateNode(fdef.node_def(i), AttrSlice(&node_attrs[i]));
    if (!s.ok()) {
      errors::AppendToMessage(&s, "In ",
                              FormatNodeDefForError(fdef.node_def(i)));
      return s;
    }
  }

  // Emits nodes for the function's return values.
  int ret_index = 0;
  for (const OpDef::ArgDef& ret_def : sig.output_arg()) {
    s = helper.AddReturnNode(ret_def, attr_values, fdef.ret(), ints_on_device,
                             &ret_index);
    if (!s.ok()) {
      errors::AppendToMessage(&s, "In function output ", Print(ret_def));
      return s;
    }
  }

  // Adds the actual node inputs using the input indexes.
  helper.AddNodeInputs();

  return Status::OK();
}