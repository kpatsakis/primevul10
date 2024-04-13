  void imDump(void *ptrs[], int &n) {
    int i = 0;
    for (auto iter = m_alloced.begin(); iter != m_alloced.end(); ++i, ++iter) {
      void *p = *iter;
      assertx(p);
      if (i < n) ptrs[i] = p;
      int ln;
      size_t size;
      memcpy(&ln, p, sizeof(ln));
      memcpy(&size, (char*)p + sizeof(ln), sizeof(size));
      printf("%d: (%p, %lu)\n", ln, p, size);
    }
    n = (i < n) ? i : n;
  }