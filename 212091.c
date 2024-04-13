  PatAndRepl(const String& pat, const String& repl)
  : pat(pat.data(), pat.size()), repl(repl.data(), repl.size()) { }