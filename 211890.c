inline StringData* StringData::Make(folly::StringPiece s) {
  return Make(s.begin(), s.size(), CopyString);
}