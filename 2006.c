llvh::Optional<NumericOrder> getNumericOrder(Literal *LHS, Literal *RHS) {
  auto *L = llvh::dyn_cast<LiteralNumber>(LHS);
  auto *R = llvh::dyn_cast<LiteralNumber>(RHS);

  if (!L || !R)
    return llvh::None;

  double l = L->getValue();
  double r = R->getValue();

  if (l < r)
    return NumericOrder::LessThan;
  if (l > r)
    return NumericOrder::GreaterThan;
  if (std::isnan(l) || std::isnan(r))
    return NumericOrder::Unordered;
  return NumericOrder::Equal;
}