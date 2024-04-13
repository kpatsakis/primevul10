my_decimal *Item::val_decimal_from_string(my_decimal *decimal_value)
{
  String *res;

  if (!(res= val_str(&str_value)))
    return 0;

  if (str2my_decimal(E_DEC_FATAL_ERROR & ~E_DEC_BAD_NUM,
                     res->ptr(), res->length(), res->charset(),
                     decimal_value) & E_DEC_BAD_NUM)
  {
    ErrConvString err(res);
    push_warning_printf(current_thd, MYSQL_ERROR::WARN_LEVEL_WARN,
                        ER_TRUNCATED_WRONG_VALUE,
                        ER(ER_TRUNCATED_WRONG_VALUE), "DECIMAL",
                        err.ptr());
  }
  return decimal_value;
}