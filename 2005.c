bool hermes::evalIsTrue(IRBuilder &builder, Literal *operand) {
  if (auto *lit = evalToBoolean(builder, operand))
    return lit->getValue();
  return false;
}