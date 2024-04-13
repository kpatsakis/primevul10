bool Item_insert_value::fix_fields(THD *thd, Item **items)
{
  DBUG_ASSERT(fixed == 0);
  /* We should only check that arg is in first table */
  if (!arg->fixed)
  {
    bool res;
    TABLE_LIST *orig_next_table= context->last_name_resolution_table;
    context->last_name_resolution_table= context->first_name_resolution_table;
    res= arg->fix_fields(thd, &arg);
    context->last_name_resolution_table= orig_next_table;
    if (res)
      return TRUE;
  }

  if (arg->type() == REF_ITEM)
    arg= static_cast<Item_ref *>(arg)->ref[0];
  if (arg->type() != FIELD_ITEM)
  {
    my_error(ER_BAD_FIELD_ERROR, MYF(0), "", "VALUES() function");
    return TRUE;
  }

  Item_field *field_arg= (Item_field *)arg;

  if (field_arg->field->table->insert_values)
  {
    Field *def_field= (Field*) sql_alloc(field_arg->field->size_of());
    if (!def_field)
      return TRUE;
    memcpy((void *)def_field, (void *)field_arg->field, field_arg->field->size_of());
    def_field->move_field_offset((my_ptrdiff_t)
                                 (def_field->table->insert_values -
                                  def_field->table->record[0]));
    set_field(def_field);
  }
  else
  {
    Field *tmp_field= field_arg->field;
    /* charset doesn't matter here, it's to avoid sigsegv only */
    tmp_field= new Field_null(0, 0, Field::NONE, field_arg->field->field_name,
                          &my_charset_bin);
    if (tmp_field)
    {
      tmp_field->init(field_arg->field->table);
      set_field(tmp_field);
      // the index is important when read bits set
      tmp_field->field_index= field_arg->field->field_index;
    }
  }
  return FALSE;
}