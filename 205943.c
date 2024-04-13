bool agg_item_charsets(DTCollation &coll, const char *fname,
                       Item **args, uint nargs, uint flags, int item_sep)
{
  if (agg_item_collations(coll, fname, args, nargs, flags, item_sep))
    return TRUE;

  return agg_item_set_converter(coll, fname, args, nargs, flags, item_sep);
}