Item_field::Item_field(Name_resolution_context *context_arg,
                       const char *db_arg,const char *table_name_arg,
                       const char *field_name_arg)
  :Item_ident(context_arg, db_arg,table_name_arg,field_name_arg),
   field(0), result_field(0), item_equal(0), no_const_subst(0),
   have_privileges(0), any_privileges(0)
{
  SELECT_LEX *select= current_thd->lex->current_select;
  collation.set(DERIVATION_IMPLICIT);
  if (select && select->parsing_place != IN_HAVING)
      select->select_n_where_fields++;
  with_field= 1;
}