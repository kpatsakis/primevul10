  ~PCRECache() {
    if (m_kind == CacheKind::Static && m_staticCache.load()) {
      DestroyStatic(m_staticCache);
    }
  }