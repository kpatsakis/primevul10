void Item_field::update_null_value() 
{ 
  /* 
    need to set no_errors to prevent warnings about type conversion 
    popping up.
  */
  THD *thd= field->table->in_use;
  int no_errors;

  no_errors= thd->no_errors;
  thd->no_errors= 1;
  Item::update_null_value();
  thd->no_errors= no_errors;
}