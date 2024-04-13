LiteralString *hermes::evalToString(IRBuilder &builder, Literal *operand) {
  if (auto *str = llvh::dyn_cast<LiteralString>(operand))
    return str;
  if (auto *num = llvh::dyn_cast<LiteralNumber>(operand)) {
    char buf[NUMBER_TO_STRING_BUF_SIZE];
    auto len = numberToString(num->getValue(), buf, sizeof(buf));
    return builder.getLiteralString(StringRef(buf, len));
  }
  return nullptr;
}