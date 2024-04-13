LiteralNumber *hermes::evalToInt32(IRBuilder &builder, Literal *operand) {
  // Eval to a number first, then truncate to a 32-bit int.
  LiteralNumber *lit = evalToNumber(builder, operand);
  if (!lit) {
    return nullptr;
  }
  double val = lit->getValue();
  return builder.getLiteralNumber(truncateToInt32(val));
}