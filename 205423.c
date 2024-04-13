int do_modify_var(struct st_command *command,
                  enum enum_operator op)
{
  const char *p= command->first_argument;
  VAR* v;
  if (!*p)
    die("Missing argument to %.*s", command->first_word_len,
        command->query);
  if (*p != '$')
    die("The argument to %.*s must be a variable (start with $)",
        command->first_word_len, command->query);
  v= var_get(p, &p, 1, 0);
  if (! v->is_int)
    die("Cannot perform inc/dec on a non-numeric value");
  switch (op) {
  case DO_DEC:
    v->int_val--;
    break;
  case DO_INC:
    v->int_val++;
    break;
  default:
    die("Invalid operator to do_modify_var");
    break;
  }
  v->int_dirty= true;
  command->last_argument= (char*)++p;
  return 0;
}