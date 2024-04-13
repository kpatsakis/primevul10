inline void StringData::checkStack() const {
  assertx(uintptr_t(this) - s_stackLimit >= s_stackSize);
}