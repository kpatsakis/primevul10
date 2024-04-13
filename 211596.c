to_usec(int64_t cycles, int64_t MHz, bool cpu_time = false)
{
  if (cpu_time) {
    return cycles / 1000;
  }
  return (cycles + MHz/2) / MHz;
}