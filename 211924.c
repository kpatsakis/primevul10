size_t PCRECache::size() const {
  switch (m_kind) {
    case CacheKind::Static:
      return m_staticCache.load(std::memory_order_acquire)->size();
    case CacheKind::Lru:
      return m_lruCache->size();
    case CacheKind::Scalable:
     return m_scalableCache->size();
  }
  always_assert(false);
}