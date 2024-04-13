void AddPrefixes(StringPiece node_name,
                 gtl::FlatSet<StringPiece, StringPieceHasher>* prefixes) {
  size_t idx = -1;
  while ((idx = node_name.find('/', idx + 1)) != StringPiece::npos) {
    prefixes->insert(node_name.substr(0, idx));
  }
}