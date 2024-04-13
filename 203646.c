command_by_name (const char *cmdname)
{
  /* Use binary search for speed.  Wget has ~100 commands, which
     guarantees a worst case performance of 7 string comparisons.  */
  int lo = 0, hi = countof (commands) - 1;

  while (lo <= hi)
    {
      int mid = (lo + hi) >> 1;
      int cmp = strcasecmp (cmdname, commands[mid].name);
      if (cmp < 0)
        hi = mid - 1;
      else if (cmp > 0)
        lo = mid + 1;
      else
        return mid;
    }
  return -1;
}