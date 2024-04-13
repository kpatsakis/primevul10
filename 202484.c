lt_dladvise_init (lt_dladvise *padvise)
{
  lt_dladvise advise = (lt_dladvise) lt__zalloc (sizeof (struct lt__advise));
  *padvise = advise;
  return (advise ? 0 : 1);
}