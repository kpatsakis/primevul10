void PCRECache::clearStatic() {
  std::unique_lock<std::mutex> lock(m_clearMutex, std::try_to_lock);
  if (!lock) return;

  auto newExpire = time(nullptr) + RuntimeOption::EvalPCREExpireInterval;
  m_expire.store(newExpire, std::memory_order_relaxed);

  auto tmpMap = CreateStatic();
  tmpMap = m_staticCache.exchange(tmpMap, std::memory_order_acq_rel);

  Treadmill::enqueue([tmpMap]() {
      DestroyStatic(tmpMap);
   });
}