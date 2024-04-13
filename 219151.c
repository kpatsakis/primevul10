ExecutionStatus Interpreter::caseIteratorBegin(
    Runtime *runtime,
    PinnedHermesValue *frameRegs,
    const inst::Inst *ip) {
  if (LLVM_LIKELY(vmisa<JSArray>(O2REG(IteratorBegin)))) {
    // Attempt to get the fast path for array iteration.
    NamedPropertyDescriptor desc;
    JSObject *propObj = JSObject::getNamedDescriptor(
        Handle<JSArray>::vmcast(&O2REG(IteratorBegin)),
        runtime,
        Predefined::getSymbolID(Predefined::SymbolIterator),
        desc);
    if (propObj) {
      HermesValue slotValue =
          JSObject::getNamedSlotValue(propObj, runtime, desc);
      if (slotValue.getRaw() == runtime->arrayPrototypeValues.getRaw()) {
        O1REG(IteratorBegin) = HermesValue::encodeNumberValue(0);
        return ExecutionStatus::RETURNED;
      }
    }
  }
  GCScopeMarkerRAII marker{runtime};
  CallResult<IteratorRecord> iterRecord =
      getIterator(runtime, Handle<>(&O2REG(IteratorBegin)));
  if (LLVM_UNLIKELY(iterRecord == ExecutionStatus::EXCEPTION)) {
    return ExecutionStatus::EXCEPTION;
  }
  O1REG(IteratorBegin) = iterRecord->iterator.getHermesValue();
  O2REG(IteratorBegin) = iterRecord->nextMethod.getHermesValue();
  return ExecutionStatus::RETURNED;
}