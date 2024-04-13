LiteralBool *hermes::evalToBoolean(IRBuilder &builder, Literal *operand) {
  bool value;
  switch (operand->getKind()) {
    case ValueKind::LiteralNullKind:
    case ValueKind::LiteralUndefinedKind:
      value = false;
      break;
    case ValueKind::LiteralBoolKind:
      value = cast<LiteralBool>(operand)->getValue();
      break;
    case ValueKind::LiteralNumberKind: {
      const auto n = cast<LiteralNumber>(operand)->getValue();
      value = !std::isnan(n) && n != 0.0;
      break;
    }
    case ValueKind::LiteralStringKind:
      value = !cast<LiteralString>(operand)->getValue().str().empty();
      break;
    default:
      return nullptr;
  }

  return builder.getLiteralBool(value);
}