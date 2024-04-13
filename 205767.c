void Hybrid_type_traits::fix_length_and_dec(Item *item, Item *arg) const
{
  item->decimals= NOT_FIXED_DEC;
  item->max_length= item->float_length(arg->decimals);
}