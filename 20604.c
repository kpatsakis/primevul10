static inline double GetFITSPixelRange(const size_t depth)
{
  return((double) ((MagickOffsetType) GetQuantumRange(depth)));
}