LiteralBool *hermes::evalToBoolean(IRBuilder &builder, Value *operand) {
  if (auto *L = llvh::dyn_cast<Literal>(operand)) {
    return evalToBoolean(builder, L);
  }

  Type OpTY = operand->getType();
  if (OpTY.isObjectType()) {
    return builder.getLiteralBool(true);
  }
  if (OpTY.isNullType() || OpTY.isUndefinedType()) {
    return builder.getLiteralBool(false);
  }
  return nullptr;
}