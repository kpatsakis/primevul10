  ~TraceWalker() {
    free(m_arcBuff);
    for (auto& r : m_recursion) delete[] r.first;
  }