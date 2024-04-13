bool Profiler::extractStats(phpret& ret, StatsMap& stats, int flags,
                            int64_t MHz)
{
    for (typename StatsMap::const_iterator iter = stats.begin();
         iter != stats.end(); ++iter) {
      returnVals(ret, iter->first, iter->second, flags, MHz);
    }
    return true;
}