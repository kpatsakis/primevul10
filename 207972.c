NEDMALLOCNOALIASATTR void   nedpfree(nedpool *p, void *mem) THROWSPEC
{
  nedpfree2(p, mem, 0);
}