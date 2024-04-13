void Profiler::returnVals(phpret& ret, const Name& name, const Counts& counts,
                          int flags, int64_t MHz)
{
    ArrayInit arr(5, ArrayInit::Map{});
    arr.set(s_ct,  counts.count);
    arr.set(s_wt,  to_usec(counts.wall_time, MHz));
    if (flags & TrackCPU) {
      arr.set(s_cpu, to_usec(counts.cpu, MHz, true));
    }
    if (flags & TrackMemory) {
      arr.set(s_mu,  counts.memory);
      arr.set(s_pmu, counts.peak_memory);
    } else if (flags & TrackMalloc) {
      arr.set(s_alloc, counts.memory);
      arr.set(s_free, counts.peak_memory);
    }
    ret.set(String(name), arr.toArray());
}