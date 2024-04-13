bool ParseAttrValueHelper_TensorNestsUnderLimit(int limit, string to_parse) {
  int nests = 0;
  int maxed_out = to_parse.length();
  int open_curly = to_parse.find('{');
  int open_bracket = to_parse.find('<');
  int close_curly = to_parse.find('}');
  int close_bracket = to_parse.find('>');
  if (open_curly == -1) {
    open_curly = maxed_out;
  }
  if (open_bracket == -1) {
    open_bracket = maxed_out;
  }
  int min = std::min(open_curly, open_bracket);
  do {
    if (open_curly == maxed_out && open_bracket == maxed_out) {
      return true;
    }
    if (min == open_curly) {
      nests += 1;
      open_curly = to_parse.find('{', open_curly + 1);
      if (open_curly == -1) {
        open_curly = maxed_out;
      }
    } else if (min == open_bracket) {
      nests += 1;
      open_bracket = to_parse.find('<', open_bracket + 1);
      if (open_bracket == -1) {
        open_bracket = maxed_out;
      }
    } else if (min == close_curly) {
      nests -= 1;
      close_curly = to_parse.find('}', close_curly + 1);
      if (close_curly == -1) {
        close_curly = maxed_out;
      }
    } else if (min == close_bracket) {
      nests -= 1;
      close_bracket = to_parse.find('>', close_bracket + 1);
      if (close_bracket == -1) {
        close_bracket = maxed_out;
      }
    }
    min = std::min({open_curly, open_bracket, close_curly, close_bracket});
  } while (nests < 100);
  return false;
}