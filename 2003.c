bool isNaN(Literal *lit) {
  if (auto *number = llvh::dyn_cast<LiteralNumber>(lit)) {
    return std::isnan(number->getValue());
  }
  return false;
}