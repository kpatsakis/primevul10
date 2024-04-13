  const NodeDef* pop() {
    CHECK(!empty());
    auto it = queue_.begin();
    const NodeDef* n = it->first;
    queue_.erase(it);
    return n;
  }