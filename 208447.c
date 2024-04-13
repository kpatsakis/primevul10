static void langmap_set_entry(int from, int to)
{
  langmap_entry_T *entries = (langmap_entry_T *)(langmap_mapga.ga_data);
  unsigned int a = 0;
  assert(langmap_mapga.ga_len >= 0);
  unsigned int b = (unsigned int)langmap_mapga.ga_len;

  /* Do a binary search for an existing entry. */
  while (a != b) {
    unsigned int i = (a + b) / 2;
    int d = entries[i].from - from;

    if (d == 0) {
      entries[i].to = to;
      return;
    }
    if (d < 0)
      a = i + 1;
    else
      b = i;
  }

  ga_grow(&langmap_mapga, 1);

  /* insert new entry at position "a" */
  entries = (langmap_entry_T *)(langmap_mapga.ga_data) + a;
  memmove(entries + 1, entries,
          ((unsigned int)langmap_mapga.ga_len - a) * sizeof(langmap_entry_T));
  ++langmap_mapga.ga_len;
  entries[0].from = from;
  entries[0].to = to;
}