static void tvDecRefRange(TypedValue* begin, TypedValue* end) {
  assertx(begin <= end);
  for (auto tv = begin; tv != end; ++tv) {
    tvDecRefGen(tv);
  }
}