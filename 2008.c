SmallString<256> buildString(const StringRef &a, const StringRef &b) {
  SmallString<256> result;
  result.append(a);
  result.append(b);
  return result;
}