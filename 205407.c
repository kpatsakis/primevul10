void append_table_headings(DYNAMIC_STRING *ds,
                           MYSQL_FIELD *field,
                           uint num_fields)
{
  uint col_idx;
  if (disable_column_names)
    return;
  for (col_idx= 0; col_idx < num_fields; col_idx++)
  {
    if (col_idx)
      dynstr_append_mem(ds, "\t", 1);
    replace_dynstr_append(ds, field[col_idx].name);
  }
  dynstr_append_mem(ds, "\n", 1);
}