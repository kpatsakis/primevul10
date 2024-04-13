bool GraphConstructor::NameExistsInGraph(StringPiece name) {
  if (existing_nodes_.find(name) != existing_nodes_.end()) return true;
  if (existing_prefixes_.find(name) != existing_prefixes_.end()) return true;
  return false;
}