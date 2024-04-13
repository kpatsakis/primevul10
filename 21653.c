bind_keyseq_to_unix_command (line)
     char *line;
{
  Keymap kmap, cmd_xmap;
  char *kseq, *value;
  int i, kstart;

  kmap = rl_get_keymap ();

  /* We duplicate some of the work done by rl_parse_and_bind here, but
     this code only has to handle `"keyseq": ["]command["]' and can
     generate an error for anything else. */
  i = isolate_sequence (line, 0, 1, &kstart);
  if (i < 0)
    return -1;

  /* Create the key sequence string to pass to rl_generic_bind */
  kseq = substring (line, kstart, i);

  for ( ; line[i] && line[i] != ':'; i++)
    ;
  if (line[i] != ':')
    {
      builtin_error (_("%s: missing colon separator"), line);
      FREE (kseq);
      return -1;
    }

  i = isolate_sequence (line, i + 1, 0, &kstart);
  if (i < 0)
    {
      FREE (kseq);
      return -1;
    }

  /* Create the value string containing the command to execute. */
  value = substring (line, kstart, i);

  /* Save the command to execute and the key sequence in the CMD_XMAP */
  cmd_xmap = get_cmd_xmap_from_keymap (kmap);
  rl_generic_bind (ISMACR, kseq, value, cmd_xmap);

  /* and bind the key sequence in the current keymap to a function that
     understands how to execute from CMD_XMAP */
  rl_bind_keyseq_in_map (kseq, bash_execute_unix_command, kmap);

  free (kseq);  
  return 0;
}