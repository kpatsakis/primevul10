set_optimize_map(regex_t* reg, OptMap* m)
{
  int i;

  for (i = 0; i < ONIG_CHAR_TABLE_SIZE; i++)
    reg->map[i] = m->map[i];

  reg->optimize   = OPTIMIZE_MAP;
  reg->dmin       = m->mmd.min;
  reg->dmax       = m->mmd.max;

  if (reg->dmin != INFINITE_LEN) {
    reg->threshold_len = reg->dmin + 1;
  }
}