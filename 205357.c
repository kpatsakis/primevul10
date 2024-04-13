static void strip_parentheses(struct st_command *command)
{
  if (strip_surrounding(command->first_argument, '(', ')'))
    die("%.*s - argument list started with '%c' must be ended with '%c'",
        command->first_word_len, command->query, '(', ')');
}