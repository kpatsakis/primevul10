Item_field::Item_field(THD *thd, Name_resolution_context *context_arg,
                       Field *f)
  :Item_ident(context_arg, f->table->s->db.str, *f->table_name, f->field_name),
   item_equal(0), no_const_subst(0),
   have_privileges(0), any_privileges(0)
{
  /*
    We always need to provide Item_field with a fully qualified field
    name to avoid ambiguity when executing prepared statements like
    SELECT * from d1.t1, d2.t1; (assuming d1.t1 and d2.t1 have columns
    with same names).
    This is because prepared statements never deal with wildcards in
    select list ('*') and always fix fields using fully specified path
    (i.e. db.table.column).
    No check for OOM: if db_name is NULL, we'll just get
    "Field not found" error.
    We need to copy db_name, table_name and field_name because they must
    be allocated in the statement memory, not in table memory (the table
    structure can go away and pop up again between subsequent executions
    of a prepared statement or after the close_tables_for_reopen() call
    in mysql_multi_update_prepare() or due to wildcard expansion in stored
    procedures).
  */
  {
    if (db_name)
      orig_db_name= thd->strdup(db_name);
    if (table_name)
      orig_table_name= thd->strdup(table_name);
    if (field_name)
      orig_field_name= thd->strdup(field_name);
    /*
      We don't restore 'name' in cleanup because it's not changed
      during execution. Still we need it to point to persistent
      memory if this item is to be reused.
    */
    name= (char*) orig_field_name;
  }
  set_field(f);
  with_field= 1;
}