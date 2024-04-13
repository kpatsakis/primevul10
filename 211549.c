void PCRECache::dump(folly::File& file) {
  switch (m_kind) {
    case CacheKind::Static:
      for (auto& it : *m_staticCache) {
        folly::writeFull(file.fd(), it.first->data(), it.first->size());
        folly::writeFull(file.fd(), "\n", 1);
      }
      break;
    case CacheKind::Lru:
    case CacheKind::Scalable:
      {
        std::vector<LRUCacheKey> keys;
        if (m_kind == CacheKind::Lru) {
          m_lruCache->snapshotKeys(keys);
        } else {
          m_scalableCache->snapshotKeys(keys);
        }
        for (auto& key: keys) {
          folly::writeFull(file.fd(), key.data(), key.size());
          folly::writeFull(file.fd(), "\n", 1);
        }
      }
      break;
  }
}