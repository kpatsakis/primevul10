  string Dep(int node_index) const {
    return strings::StrCat("^", Name(node_index));
  }