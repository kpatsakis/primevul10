void Item_ident_for_show::make_field(Send_field *tmp_field)
{
  tmp_field->table_name= tmp_field->org_table_name= table_name;
  tmp_field->db_name= db_name;
  tmp_field->col_name= tmp_field->org_col_name= field->field_name;
  tmp_field->charsetnr= field->charset()->number;
  tmp_field->length=field->field_length;
  tmp_field->type=field->type();
  tmp_field->flags= field->table->maybe_null ? 
    (field->flags & ~NOT_NULL_FLAG) : field->flags;
  tmp_field->decimals= field->decimals();
}