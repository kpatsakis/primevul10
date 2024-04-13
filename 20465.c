is_left(int a)
{
  if (a == ANCHOR_END_BUF  || a == ANCHOR_SEMI_END_BUF ||
      a == ANCHOR_END_LINE || a == ANCHOR_PREC_READ || a == ANCHOR_PREC_READ_NOT)
    return 0;

  return 1;
}