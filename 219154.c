ExecutionStatus Interpreter::casePutOwnByVal(
    Runtime *runtime,
    PinnedHermesValue *frameRegs,
    const Inst *ip) {
  return JSObject::defineOwnComputed(
             Handle<JSObject>::vmcast(&O1REG(PutOwnByVal)),
             runtime,
             Handle<>(&O3REG(PutOwnByVal)),
             ip->iPutOwnByVal.op4
                 ? DefinePropertyFlags::getDefaultNewPropertyFlags()
                 : DefinePropertyFlags::getNewNonEnumerableFlags(),
             Handle<>(&O2REG(PutOwnByVal)))
      .getStatus();
}