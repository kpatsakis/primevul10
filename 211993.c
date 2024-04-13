  PCRECache()
    : m_kind(CacheKind::Static), m_staticCache(nullptr)
  {
    reinit(CacheKind::Static);
  }