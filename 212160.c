void PCRECache::DestroyStatic(StaticCache* cache) {
  // We delete uncounted keys while iterating the cache, which is OK for
  // AtomicHashArray, but not OK for other containers, such as
  // std::unordered_map.  If you change the cache type make sure that property
  // holds or fix this function.
  static_assert(std::is_same<PCRECache::StaticCache,
      folly::AtomicHashArray<const StringData*, const pcre_cache_entry*,
                             string_data_hash, ahm_string_data_same>>::value,
      "StaticCache must be an AtomicHashArray or this destructor is wrong.");
  for (auto& it : *cache) {
    if (it.first->isUncounted()) {
      StringData::ReleaseUncounted(it.first);
    }
    delete it.second;
  }
  StaticCache::destroy(cache);
}