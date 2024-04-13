void PCRECache::reinit(CacheKind kind) {
  switch (m_kind) {
    case CacheKind::Static:
      if (m_staticCache.load()) {
        DestroyStatic(m_staticCache);
        m_staticCache = nullptr;
      }
      break;
    case CacheKind::Lru:
      m_lruCache.reset();
      break;
    case CacheKind::Scalable:
      m_scalableCache.reset();
      break;
  }
  m_kind = kind;

  switch (kind) {
    case CacheKind::Static:
      m_staticCache = CreateStatic();
      m_expire = time(nullptr) + RuntimeOption::EvalPCREExpireInterval;
      break;
    case CacheKind::Lru:
      m_lruCache.reset(new LRUCache(RuntimeOption::EvalPCRETableSize));
      break;
    case CacheKind::Scalable:
      m_scalableCache.reset(
        new ScalableCache(RuntimeOption::EvalPCRETableSize));
      break;
  }
}