bool HHVM_FUNCTION(HH_is_meth_caller, TypedValue v) {
  if (tvIsFunc(v)) {
    return val(v).pfunc->isMethCaller();
  } else if (tvIsObject(v)) {
    auto const mcCls = Class::lookup(s_meth_caller_cls.get());
    auto const dynMcCls = Class::lookup(s_dyn_meth_caller_cls.get());
    auto const cls = val(v).pobj->getVMClass();
    assertx(mcCls);
    assertx(dynMcCls);
    return mcCls == cls || dynMcCls == cls;
  }
  return false;
}