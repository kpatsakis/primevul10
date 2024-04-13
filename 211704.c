inline folly::MutableStringPiece StringData::bufferSlice() {
  assertx(!isImmutable());
  return folly::MutableStringPiece{mutableData(), capacity()};
}