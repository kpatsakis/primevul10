bool pcre_literal_data::isLiteral() const {
  return literal_str.has_value();
}