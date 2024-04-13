PCRECache::StaticCache* PCRECache::CreateStatic() {
  StaticCache::Config config;
  config.maxLoadFactor = 0.5;
  return StaticCache::create(
      RuntimeOption::EvalPCRETableSize, config).release();
}