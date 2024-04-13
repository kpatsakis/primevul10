void free_used_memory()
{
  uint i;
  DBUG_ENTER("free_used_memory");

  if (connections)
    close_connections();
  close_files();
  my_hash_free(&var_hash);

  for (i= 0 ; i < q_lines.elements ; i++)
  {
    struct st_command **q= dynamic_element(&q_lines, i, struct st_command**);
    my_free((*q)->query_buf);
    if ((*q)->eval_query.str)
      dynstr_free(&(*q)->eval_query);
    if ((*q)->content.str)
      dynstr_free(&(*q)->content);
    my_free((*q));
  }
  for (i= 0; i < 10; i++)
  {
    if (var_reg[i].alloced_len)
      my_free(var_reg[i].str_val);
  }
  while (embedded_server_arg_count > 1)
    my_free(embedded_server_args[--embedded_server_arg_count]);
  delete_dynamic(&q_lines);
  dynstr_free(&ds_res);
  if (ds_warn)
    dynstr_free(ds_warn);
  free_all_replace();
  my_free(opt_pass);
  free_defaults(default_argv);
  free_root(&require_file_root, MYF(0));
  free_re();
#ifdef __WIN__
  free_tmp_sh_file();
  free_win_path_patterns();
#endif
  DBUG_VOID_RETURN;
}