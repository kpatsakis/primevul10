void Item_trigger_field::setup_field(THD *thd, TABLE *table,
                                     GRANT_INFO *table_grant_info)
{
  /*
    It is too early to mark fields used here, because before execution
    of statement that will invoke trigger other statements may use same
    TABLE object, so all such mark-up will be wiped out.
    So instead we do it in Table_triggers_list::mark_fields_used()
    method which is called during execution of these statements.
  */
  enum_mark_columns save_mark_used_columns= thd->mark_used_columns;
  thd->mark_used_columns= MARK_COLUMNS_NONE;
  /*
    Try to find field by its name and if it will be found
    set field_idx properly.
  */
  (void)find_field_in_table(thd, table, field_name, (uint) strlen(field_name),
                            0, &field_idx);
  thd->mark_used_columns= save_mark_used_columns;
  triggers= table->triggers;
  table_grants= table_grant_info;
}