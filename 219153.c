ExecutionStatus Interpreter::caseDirectEval(
    Runtime *runtime,
    PinnedHermesValue *frameRegs,
    const Inst *ip) {
  auto *result = &O1REG(DirectEval);
  auto *input = &O2REG(DirectEval);

  GCScopeMarkerRAII gcMarker{runtime};

  // Check to see if global eval() has been overriden, in which case call it as
  // as normal function.
  auto global = runtime->getGlobal();
  auto existingEval = global->getNamed_RJS(
      global, runtime, Predefined::getSymbolID(Predefined::eval));
  if (LLVM_UNLIKELY(existingEval == ExecutionStatus::EXCEPTION)) {
    return ExecutionStatus::EXCEPTION;
  }
  auto *nativeExistingEval = dyn_vmcast<NativeFunction>(existingEval->get());
  if (LLVM_UNLIKELY(
          !nativeExistingEval ||
          nativeExistingEval->getFunctionPtr() != hermes::vm::eval)) {
    if (auto *existingEvalCallable =
            dyn_vmcast<Callable>(existingEval->get())) {
      auto evalRes = existingEvalCallable->executeCall1(
          runtime->makeHandle<Callable>(existingEvalCallable),
          runtime,
          Runtime::getUndefinedValue(),
          *input);
      if (LLVM_UNLIKELY(evalRes == ExecutionStatus::EXCEPTION)) {
        return ExecutionStatus::EXCEPTION;
      }
      *result = evalRes->get();
      evalRes->invalidate();
      return ExecutionStatus::RETURNED;
    }
    return runtime->raiseTypeErrorForValue(
        runtime->makeHandle(std::move(*existingEval)), " is not a function");
  }

  if (!input->isString()) {
    *result = *input;
    return ExecutionStatus::RETURNED;
  }

  // Create a dummy scope, so that the local eval executes in its own scope
  // (as per the spec for strict callers, which is the only thing we support).

  ScopeChain scopeChain{};
  scopeChain.functions.emplace_back();

  auto cr = vm::directEval(
      runtime, Handle<StringPrimitive>::vmcast(input), scopeChain, false);
  if (cr == ExecutionStatus::EXCEPTION)
    return ExecutionStatus::EXCEPTION;

  *result = *cr;
  return ExecutionStatus::RETURNED;
}