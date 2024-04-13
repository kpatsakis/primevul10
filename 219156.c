ExecutionStatus Interpreter::casePutOwnGetterSetterByVal(
    Runtime *runtime,
    PinnedHermesValue *frameRegs,
    const inst::Inst *ip) {
  DefinePropertyFlags dpFlags{};
  dpFlags.setConfigurable = 1;
  dpFlags.configurable = 1;
  dpFlags.setEnumerable = 1;
  dpFlags.enumerable = ip->iPutOwnGetterSetterByVal.op5;

  MutableHandle<Callable> getter(runtime);
  MutableHandle<Callable> setter(runtime);
  if (LLVM_LIKELY(!O3REG(PutOwnGetterSetterByVal).isUndefined())) {
    dpFlags.setGetter = 1;
    getter = vmcast<Callable>(O3REG(PutOwnGetterSetterByVal));
  }
  if (LLVM_LIKELY(!O4REG(PutOwnGetterSetterByVal).isUndefined())) {
    dpFlags.setSetter = 1;
    setter = vmcast<Callable>(O4REG(PutOwnGetterSetterByVal));
  }
  assert(
      (dpFlags.setSetter || dpFlags.setGetter) &&
      "No accessor set in PutOwnGetterSetterByVal");

  auto res = PropertyAccessor::create(runtime, getter, setter);
  if (LLVM_UNLIKELY(res == ExecutionStatus::EXCEPTION))
    return ExecutionStatus::EXCEPTION;

  auto accessor = runtime->makeHandle<PropertyAccessor>(*res);

  return JSObject::defineOwnComputed(
             Handle<JSObject>::vmcast(&O1REG(PutOwnGetterSetterByVal)),
             runtime,
             Handle<>(&O2REG(PutOwnGetterSetterByVal)),
             dpFlags,
             accessor)
      .getStatus();
}