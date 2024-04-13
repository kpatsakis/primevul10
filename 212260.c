bool PCRECache::find(Accessor& accessor,
                     const StringData* regex,
                     TempKeyCache& keyCache)
{
  switch (m_kind) {
    case CacheKind::Static:
      {
        assertx(m_staticCache.load());
        StaticCache::iterator it;
        auto cache = m_staticCache.load(std::memory_order_acquire);
        if ((it = cache->find(regex)) != cache->end()) {
          accessor = it->second;
          return true;
        }
        return false;
      }
    case CacheKind::Lru:
    case CacheKind::Scalable:
      {
        if (!keyCache) {
          keyCache.reset(new LRUCacheKey(regex->data(), regex->size()));
        }
        bool found;
        if (m_kind == CacheKind::Lru) {
          found = m_lruCache->find(accessor.resetToLRU(), *keyCache);
        } else {
          found = m_scalableCache->find(accessor.resetToLRU(), *keyCache);
        }
        return found;
      }
  }
  always_assert(false);
}