bool cmp_items(Item *a, Item *b)
{
  return a->eq(b, FALSE);
}