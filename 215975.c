bool GraphConstructor::NameExistsInGraphDef(StringPiece name) {
  if (gdef_nodes_.find(name) != gdef_nodes_.end()) return true;
  if (gdef_prefixes_.find(name) != gdef_prefixes_.end()) return true;
  return false;
}