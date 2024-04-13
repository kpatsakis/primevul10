alt_merge_mml(MinMax* to, MinMax* from)
{
  if (to->min > from->min) to->min = from->min;
  if (to->max < from->max) to->max = from->max;
}