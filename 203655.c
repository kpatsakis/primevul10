decode_string (const char *val, const struct decode_item *items, int itemcount,
               int *place)
{
  int i;
  for (i = 0; i < itemcount; i++)
    if (0 == strcasecmp (val, items[i].name))
      {
        *place = items[i].code;
        return true;
      }
  return false;
}