void PCRECache::insert(
  Accessor& accessor,
  const StringData* regex,
  TempKeyCache& keyCache,
  const pcre_cache_entry* ent
) {
  switch (m_kind) {
    case CacheKind::Static:
      {
        assertx(m_staticCache.load());
        // Clear the cache if we haven't refreshed it in a while
        if (time(nullptr) > m_expire) {
          clearStatic();
        }
        auto const cache = m_staticCache.load(std::memory_order_acquire);
        auto const key =
          regex->isStatic() ||
          (regex->isUncounted() && regex->uncountedIncRef()) ?
          regex : StringData::MakeUncounted(regex->slice());
        auto pair = cache->insert(StaticCachePair(key, ent));
        if (pair.second) {
          // Inserted, container owns the pointer
          accessor = ent;
        } else {
          // Not inserted, caller needs to own the pointer
          if (regex->isUncounted()) StringData::ReleaseUncounted(key);
          accessor = EntryPtr(ent);
        }
      }
      break;
    case CacheKind::Lru:
    case CacheKind::Scalable:
      {
        if (!keyCache) {
          keyCache.reset(new LRUCacheKey(regex->data(), regex->size()));
        }
        // Pointer ownership is shared between container and caller
        accessor = EntryPtr(ent);
        if (m_kind == CacheKind::Lru) {
          m_lruCache->insert(*keyCache, accessor.entryPtr());
        } else {
          m_scalableCache->insert(*keyCache, accessor.entryPtr());
        }
      }
      break;
  }
}