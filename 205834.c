Item_param::set_value(THD *thd, sp_rcontext *ctx, Item **it)
{
  Item *arg= *it;

  if (arg->is_null())
  {
    set_null();
    return FALSE;
  }

  null_value= FALSE;

  switch (arg->result_type()) {
  case STRING_RESULT:
  {
    char str_buffer[STRING_BUFFER_USUAL_SIZE];
    String sv_buffer(str_buffer, sizeof(str_buffer), &my_charset_bin);
    String *sv= arg->val_str(&sv_buffer);

    if (!sv)
      return TRUE;

    set_str(sv->c_ptr_safe(), sv->length());
    str_value_ptr.set(str_value.ptr(),
                      str_value.length(),
                      str_value.charset());
    collation.set(str_value.charset(), DERIVATION_COERCIBLE);
    decimals= 0;

    break;
  }

  case REAL_RESULT:
    set_double(arg->val_real());
    break;

  case INT_RESULT:
    set_int(arg->val_int(), arg->max_length);
    break;

  case DECIMAL_RESULT:
  {
    my_decimal dv_buf;
    my_decimal *dv= arg->val_decimal(&dv_buf);

    if (!dv)
      return TRUE;

    set_decimal(dv);
    break;
  }

  default:
    /* That can not happen. */

    DBUG_ASSERT(TRUE);  // Abort in debug mode.

    set_null();         // Set to NULL in release mode.
    return FALSE;
  }

  item_result_type= arg->result_type();
  item_type= arg->type();
  return FALSE;
}