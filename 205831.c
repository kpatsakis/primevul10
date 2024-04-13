void Item_field::set_field(Field *field_par)
{
  field=result_field=field_par;			// for easy coding with fields
  maybe_null=field->maybe_null();
  decimals= field->decimals();
  table_name= *field_par->table_name;
  field_name= field_par->field_name;
  db_name= field_par->table->s->db.str;
  alias_name_used= field_par->table->alias_name_used;
  unsigned_flag=test(field_par->flags & UNSIGNED_FLAG);
  collation.set(field_par->charset(), field_par->derivation(),
                field_par->repertoire());
  fix_char_length(field_par->char_length());

  max_length= adjust_max_effective_column_length(field_par, max_length);

  fixed= 1;
  if (field->table->s->tmp_table == SYSTEM_TMP_TABLE)
    any_privileges= 0;
}