int Item_default_value::save_in_field(Field *field_arg, bool no_conversions)
{
  if (!arg)
  {
    if (field_arg->flags & NO_DEFAULT_VALUE_FLAG &&
        field_arg->real_type() != MYSQL_TYPE_ENUM)
    {
      if (field_arg->reset())
      {
        my_message(ER_CANT_CREATE_GEOMETRY_OBJECT,
                   ER(ER_CANT_CREATE_GEOMETRY_OBJECT), MYF(0));
        return -1;
      }

      if (context->error_processor == &view_error_processor)
      {
        TABLE_LIST *view= field_arg->table->pos_in_table_list->top_table();
        push_warning_printf(field_arg->table->in_use,
                            MYSQL_ERROR::WARN_LEVEL_WARN,
                            ER_NO_DEFAULT_FOR_VIEW_FIELD,
                            ER(ER_NO_DEFAULT_FOR_VIEW_FIELD),
                            view->view_db.str,
                            view->view_name.str);
      }
      else
      {
        push_warning_printf(field_arg->table->in_use,
                            MYSQL_ERROR::WARN_LEVEL_WARN,
                            ER_NO_DEFAULT_FOR_FIELD,
                            ER(ER_NO_DEFAULT_FOR_FIELD),
                            field_arg->field_name);
      }
      return 1;
    }
    field_arg->set_default();
    return 0;
  }
  return Item_field::save_in_field(field_arg, no_conversions);
}