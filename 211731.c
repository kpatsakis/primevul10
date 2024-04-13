void pcre_reinit() {
  PCRECache::CacheKind kind;
  if (RuntimeOption::EvalPCRECacheType == "static") {
    kind = PCRECache::CacheKind::Static;
  } else if (RuntimeOption::EvalPCRECacheType == "lru") {
    kind = PCRECache::CacheKind::Lru;
  } else if (RuntimeOption::EvalPCRECacheType == "scalable") {
    kind = PCRECache::CacheKind::Scalable;
  } else {
    Logger::Warning("Eval.PCRECacheType should be either static, "
                    "lru or scalable");
    kind = PCRECache::CacheKind::Scalable;
  }
  s_pcreCache.reinit(kind);
}