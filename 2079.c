const FunctionDef* FunctionLibraryDefinition::GetAttrImpl(
    const NodeDef& ndef) const {
  if (ndef.op() != kGradientOp) {
    // If 'ndef' calls a function and the function's def has the attr,
    // returns it.
    return Find(ndef.op());
  }

  // If ndef is SymbolicGradient[f=Foo], we use Foo's gradient or
  // Foo's attributes.
  const NameAttrList* forward_func_attrs;
  if (!TryGetNodeAttr(ndef, kFuncAttr, &forward_func_attrs)) {
    return nullptr;
  }
  const string& func_name = forward_func_attrs->name();
  {
    tf_shared_lock l(mu_);
    const string& grad_name = FindGradientHelper(func_name);
    // If 'func' has a user-defined gradient function, uses the grad
    // function's attrs to see if noinline is specified. Otherwise,
    // uses func's attrs.
    if (!grad_name.empty()) {
      if (const auto helper = FindHelper(grad_name)) {
        return &(helper->fdef);
      } else {
        return nullptr;
      }
    }
    if (const auto helper = FindHelper(func_name)) {
      return &(helper->fdef);
    } else {
      return nullptr;
    }
  }
}