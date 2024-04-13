Item_type_holder::Item_type_holder(THD *thd, Item *item)
  :Item(thd, item), enum_set_typelib(0), fld_type(get_real_type(item))
{
  DBUG_ASSERT(item->fixed);
  maybe_null= item->maybe_null;
  collation.set(item->collation);
  get_full_info(item);
  /* fix variable decimals which always is NOT_FIXED_DEC */
  if (Field::result_merge_type(fld_type) == INT_RESULT)
    decimals= 0;
  prev_decimal_int_part= item->decimal_int_part();
#ifdef HAVE_SPATIAL
  if (item->field_type() == MYSQL_TYPE_GEOMETRY)
    geometry_type= item->get_geometry_type();
#endif /* HAVE_SPATIAL */
}