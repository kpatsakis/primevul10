  void requestInit() override {
#ifdef IM_MEMORY_CHECK
    void *ptrs[1000];
    int n = 1000;
    if (m_mallocSize) imDump(ptrs, n);
#endif
    assertx(m_mallocSize == 0);
    m_mallocSize = 0;
  }