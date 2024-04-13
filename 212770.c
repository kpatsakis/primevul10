CallResult<bool> proxyOpFlags(
    Runtime *runtime,
    PropOpFlags opFlags,
    const char *msg,
    CallResult<bool> res) {
  if (LLVM_UNLIKELY(res == ExecutionStatus::EXCEPTION)) {
    return ExecutionStatus::EXCEPTION;
  }
  if (!*res && opFlags.getThrowOnError()) {
    return runtime->raiseTypeError(msg);
  }
  return res;
}