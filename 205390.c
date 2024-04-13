void append_stmt_result(DYNAMIC_STRING *ds, MYSQL_STMT *stmt,
                        MYSQL_FIELD *fields, uint num_fields)
{
  MYSQL_BIND *my_bind;
  my_bool *is_null;
  ulong *length;
  uint i;
  int error;

  /* Allocate array with bind structs, lengths and NULL flags */
  my_bind= (MYSQL_BIND*) my_malloc(num_fields * sizeof(MYSQL_BIND),
				MYF(MY_WME | MY_FAE | MY_ZEROFILL));
  length= (ulong*) my_malloc(num_fields * sizeof(ulong),
			     MYF(MY_WME | MY_FAE));
  is_null= (my_bool*) my_malloc(num_fields * sizeof(my_bool),
				MYF(MY_WME | MY_FAE));

  /* Allocate data for the result of each field */
  for (i= 0; i < num_fields; i++)
  {
    uint max_length= fields[i].max_length + 1;
    my_bind[i].buffer_type= MYSQL_TYPE_STRING;
    my_bind[i].buffer= my_malloc(max_length, MYF(MY_WME | MY_FAE));
    my_bind[i].buffer_length= max_length;
    my_bind[i].is_null= &is_null[i];
    my_bind[i].length= &length[i];

    DBUG_PRINT("bind", ("col[%d]: buffer_type: %d, buffer_length: %lu",
			i, my_bind[i].buffer_type, my_bind[i].buffer_length));
  }

  if (mysql_stmt_bind_result(stmt, my_bind))
    die("mysql_stmt_bind_result failed: %d: %s",
	mysql_stmt_errno(stmt), mysql_stmt_error(stmt));

  while ((error=mysql_stmt_fetch(stmt)) == 0)
  {
    for (i= 0; i < num_fields; i++)
      append_field(ds, i, &fields[i], (char*)my_bind[i].buffer,
                   *my_bind[i].length, *my_bind[i].is_null);
    if (!display_result_vertically)
      dynstr_append_mem(ds, "\n", 1);
  }

  if (error != MYSQL_NO_DATA)
    die("mysql_fetch didn't end with MYSQL_NO_DATA from statement: "
        "error: %d", error);
  if (mysql_stmt_fetch(stmt) != MYSQL_NO_DATA)
    die("mysql_fetch didn't end with MYSQL_NO_DATA from statement: %d %s",
	mysql_stmt_errno(stmt), mysql_stmt_error(stmt));

  for (i= 0; i < num_fields; i++)
  {
    /* Free data for output */
    my_free(my_bind[i].buffer);
  }
  /* Free array with bind structs, lengths and NULL flags */
  my_free(my_bind);
  my_free(length);
  my_free(is_null);
}