LiteralNumber *hermes::evalToNumber(IRBuilder &builder, Literal *operand) {
  if (auto *numLiteral = llvh::dyn_cast<LiteralNumber>(operand)) {
    return numLiteral;
  }
  if (auto *boolLiteral = llvh::dyn_cast<LiteralBool>(operand)) {
    return builder.getLiteralNumber(boolLiteral->getValue());
  }
  if (operand->getType().isUndefinedType()) {
    return builder.getLiteralNaN();
  }
  if (operand->getType().isNullType()) {
    return builder.getLiteralPositiveZero();
  }
  return nullptr;
}