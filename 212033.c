inline StringData* StringData::Make(const StringData* s1,
                                    folly::StringPiece s2) {
  return Make(s1->slice(), s2);
}