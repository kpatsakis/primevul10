  void requestShutdown() override {
#ifdef IM_MEMORY_CHECK
    void *ptrs[1000];
    int n = 1000;
    if (m_mallocSize) imDump(ptrs, n);
    assertx(m_mallocSize == 0);
#endif
    m_mallocSize = 0;
  }