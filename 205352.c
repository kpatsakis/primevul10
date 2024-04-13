void do_get_errcodes(struct st_command *command)
{
  struct st_match_err *to= saved_expected_errors.err;
  DBUG_ENTER("do_get_errcodes");

  if (!*command->first_argument)
    die("Missing argument(s) to 'error'");

  /* TODO: Potentially, there is a possibility of variables 
     being expanded twice, e.g.

     let $errcodes = 1,\$a;
     let $a = 1051;
     error $errcodes;
     DROP TABLE unknown_table;
     ...
     Got one of the listed errors

     But since it requires manual escaping, it does not seem 
     particularly dangerous or error-prone. 
  */
  DYNAMIC_STRING ds;
  init_dynamic_string(&ds, 0, command->query_len + 64, 256);
  do_eval(&ds, command->first_argument, command->end, !is_windows);
  char *p= ds.str;

  uint count= 0;
  char *next;

  do
  {
    char *end;

    /* Skip leading spaces */
    while (*p && *p == ' ')
      p++;

    /* Find end */
    end= p;
    while (*end && *end != ',' && *end != ' ')
      end++;

    next=end;

    /* code to handle variables passed to mysqltest */
     if( *p == '$')
     {
        const char* fin;
        VAR *var = var_get(p,&fin,0,0);
        p=var->str_val;
        end=p+var->str_val_len;
     }

    if (*p == 'S')
    {
      char *to_ptr= to->code.sqlstate;

      /*
        SQLSTATE string
        - Must be SQLSTATE_LENGTH long
        - May contain only digits[0-9] and _uppercase_ letters
      */
      p++; /* Step past the S */
      if ((end - p) != SQLSTATE_LENGTH)
        die("The sqlstate must be exactly %d chars long", SQLSTATE_LENGTH);

      /* Check sqlstate string validity */
      while (*p && p < end)
      {
        if (my_isdigit(charset_info, *p) || my_isupper(charset_info, *p))
          *to_ptr++= *p++;
        else
          die("The sqlstate may only consist of digits[0-9] " \
              "and _uppercase_ letters");
      }

      *to_ptr= 0;
      to->type= ERR_SQLSTATE;
      DBUG_PRINT("info", ("ERR_SQLSTATE: %s", to->code.sqlstate));
    }
    else if (*p == 's')
    {
      die("The sqlstate definition must start with an uppercase S");
    }
    else if (*p == 'E' || *p == 'H')
    {
      /* Error name string */

      DBUG_PRINT("info", ("Error name: %s", p));
      to->code.errnum= get_errcode_from_name(p, end);
      to->type= ERR_ERRNO;
      DBUG_PRINT("info", ("ERR_ERRNO: %d", to->code.errnum));
    }
    else if (*p == 'e' || *p == 'h')
    {
      die("The error name definition must start with an uppercase E or H");
    }
    else
    {
      long val;
      char *start= p;
      /* Check that the string passed to str2int only contain digits */
      while (*p && p != end)
      {
        if (!my_isdigit(charset_info, *p))
          die("Invalid argument to error: '%s' - "            \
              "the errno may only consist of digits[0-9]",
              command->first_argument);
        p++;
      }

      /* Convert the sting to int */
      if (!str2int(start, 10, (long) INT_MIN, (long) INT_MAX, &val))
	die("Invalid argument to error: '%s'", command->first_argument);

      to->code.errnum= (uint) val;
      to->type= ERR_ERRNO;
      DBUG_PRINT("info", ("ERR_ERRNO: %d", to->code.errnum));
    }
    to++;
    count++;

    if (count >= (sizeof(saved_expected_errors.err) /
                  sizeof(struct st_match_err)))
      die("Too many errorcodes specified");

    /* Set pointer to the end of the last error code */
    p= next;

    /* Find next ',' */
    while (*p && *p != ',')
      p++;

    if (*p)
      p++; /* Step past ',' */

  } while (*p);

  command->last_argument= command->first_argument;
  while (*command->last_argument)
    command->last_argument++;
  
  to->type= ERR_EMPTY;                        /* End of data */

  DBUG_PRINT("info", ("Expected errors: %d", count));
  saved_expected_errors.count= count;
  dynstr_free(&ds);
  DBUG_VOID_RETURN;
}