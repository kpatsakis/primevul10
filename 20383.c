int_multiply_cmp(int x, int y, int v)
{
  if (x == 0 || y == 0) return -1;

  if (x < INT_MAX / y) {
    int xy = x * y;
    if (xy > v) return 1;
    else {
      if (xy == v) return 0;
      else return -1;
    }
  }
  else
    return 1;
}