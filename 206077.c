bool agg_item_collations(DTCollation &c, const char *fname,
                         Item **av, uint count, uint flags, int item_sep)
{
  uint i;
  Item **arg;
  bool unknown_cs= 0;

  c.set(av[0]->collation);
  for (i= 1, arg= &av[item_sep]; i < count; i++, arg+= item_sep)
  {
    if (c.aggregate((*arg)->collation, flags))
    {
      if (c.derivation == DERIVATION_NONE &&
          c.collation == &my_charset_bin)
      {
        unknown_cs= 1;
        continue;
      }
      my_coll_agg_error(av, count, fname, item_sep);
      return TRUE;
    }
  }

  if (unknown_cs &&
      c.derivation != DERIVATION_EXPLICIT)
  {
    my_coll_agg_error(av, count, fname, item_sep);
    return TRUE;
  }

  if ((flags & MY_COLL_DISALLOW_NONE) &&
      c.derivation == DERIVATION_NONE)
  {
    my_coll_agg_error(av, count, fname, item_sep);
    return TRUE;
  }
  
  /* If all arguments where numbers, reset to @@collation_connection */
  if (flags & MY_COLL_ALLOW_NUMERIC_CONV &&
      c.derivation == DERIVATION_NUMERIC)
    c.set(Item::default_charset(), DERIVATION_COERCIBLE, MY_REPERTOIRE_NUMERIC);

  return FALSE;
}