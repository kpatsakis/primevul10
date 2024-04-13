Item_field::Item_field(Field *f)
  :Item_ident(0, NullS, *f->table_name, f->field_name),
   item_equal(0), no_const_subst(0),
   have_privileges(0), any_privileges(0)
{
  set_field(f);
  /*
    field_name and table_name should not point to garbage
    if this item is to be reused
  */
  orig_table_name= orig_field_name= "";
  with_field= 1;
}