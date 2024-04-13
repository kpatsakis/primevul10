unsigned countOccurences(const std::string& of, const std::string& in) {
  unsigned occurences = 0;
  std::string::size_type lastOccurence = -1;
  while ((lastOccurence = in.find(of, lastOccurence + 1)) !=
         std::string::npos) {
    occurences++;
  }
  return occurences;
}