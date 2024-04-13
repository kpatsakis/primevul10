initialize_readline ()
{
  rl_command_func_t *func;
  char kseq[2];

  if (bash_readline_initialized)
    return;

  rl_terminal_name = get_string_value ("TERM");
  rl_instream = stdin;
  rl_outstream = stderr;

  /* Allow conditional parsing of the ~/.inputrc file. */
  rl_readline_name = "Bash";

  /* Add bindable names before calling rl_initialize so they may be
     referenced in the various inputrc files. */
  rl_add_defun ("shell-expand-line", shell_expand_line, -1);
#ifdef BANG_HISTORY
  rl_add_defun ("history-expand-line", history_expand_line, -1);
  rl_add_defun ("magic-space", tcsh_magic_space, -1);
#endif

  rl_add_defun ("shell-forward-word", bash_forward_shellword, -1);
  rl_add_defun ("shell-backward-word", bash_backward_shellword, -1);
  rl_add_defun ("shell-kill-word", bash_kill_shellword, -1);
  rl_add_defun ("shell-backward-kill-word", bash_backward_kill_shellword, -1);
  rl_add_defun ("shell-transpose-words", bash_transpose_shellwords, -1);

#ifdef ALIAS
  rl_add_defun ("alias-expand-line", alias_expand_line, -1);
#  ifdef BANG_HISTORY
  rl_add_defun ("history-and-alias-expand-line", history_and_alias_expand_line, -1);
#  endif
#endif

  /* Backwards compatibility. */
  rl_add_defun ("insert-last-argument", rl_yank_last_arg, -1);

  rl_add_defun ("operate-and-get-next", operate_and_get_next, -1);
  rl_add_defun ("display-shell-version", display_shell_version, -1);
  rl_add_defun ("edit-and-execute-command", emacs_edit_and_execute_command, -1);

#if defined (BRACE_COMPLETION)
  rl_add_defun ("complete-into-braces", bash_brace_completion, -1);
#endif

#if defined (SPECIFIC_COMPLETION_FUNCTIONS)
  rl_add_defun ("complete-filename", bash_complete_filename, -1);
  rl_add_defun ("possible-filename-completions", bash_possible_filename_completions, -1);
  rl_add_defun ("complete-username", bash_complete_username, -1);
  rl_add_defun ("possible-username-completions", bash_possible_username_completions, -1);
  rl_add_defun ("complete-hostname", bash_complete_hostname, -1);
  rl_add_defun ("possible-hostname-completions", bash_possible_hostname_completions, -1);
  rl_add_defun ("complete-variable", bash_complete_variable, -1);
  rl_add_defun ("possible-variable-completions", bash_possible_variable_completions, -1);
  rl_add_defun ("complete-command", bash_complete_command, -1);
  rl_add_defun ("possible-command-completions", bash_possible_command_completions, -1);
  rl_add_defun ("glob-complete-word", bash_glob_complete_word, -1);
  rl_add_defun ("glob-expand-word", bash_glob_expand_word, -1);
  rl_add_defun ("glob-list-expansions", bash_glob_list_expansions, -1);
#endif

  rl_add_defun ("dynamic-complete-history", dynamic_complete_history, -1);
  rl_add_defun ("dabbrev-expand", bash_dabbrev_expand, -1);

  /* Bind defaults before binding our custom shell keybindings. */
  if (RL_ISSTATE(RL_STATE_INITIALIZED) == 0)
    rl_initialize ();

  /* Bind up our special shell functions. */
  rl_bind_key_if_unbound_in_map (CTRL('E'), shell_expand_line, emacs_meta_keymap);

#ifdef BANG_HISTORY
  rl_bind_key_if_unbound_in_map ('^', history_expand_line, emacs_meta_keymap);
#endif

  rl_bind_key_if_unbound_in_map (CTRL ('O'), operate_and_get_next, emacs_standard_keymap);
  rl_bind_key_if_unbound_in_map (CTRL ('V'), display_shell_version, emacs_ctlx_keymap);

  /* In Bash, the user can switch editing modes with "set -o [vi emacs]",
     so it is not necessary to allow C-M-j for context switching.  Turn
     off this occasionally confusing behaviour. */
  kseq[0] = CTRL('J');
  kseq[1] = '\0';
  func = rl_function_of_keyseq (kseq, emacs_meta_keymap, (int *)NULL);
  if (func == rl_vi_editing_mode)
    rl_unbind_key_in_map (CTRL('J'), emacs_meta_keymap);
  kseq[0] = CTRL('M');
  func = rl_function_of_keyseq (kseq, emacs_meta_keymap, (int *)NULL);
  if (func == rl_vi_editing_mode)
    rl_unbind_key_in_map (CTRL('M'), emacs_meta_keymap);
#if defined (VI_MODE)
  kseq[0] = CTRL('E');
  func = rl_function_of_keyseq (kseq, vi_movement_keymap, (int *)NULL);
  if (func == rl_emacs_editing_mode)
    rl_unbind_key_in_map (CTRL('E'), vi_movement_keymap);
#endif

#if defined (BRACE_COMPLETION)
  rl_bind_key_if_unbound_in_map ('{', bash_brace_completion, emacs_meta_keymap); /*}*/
#endif /* BRACE_COMPLETION */

#if defined (SPECIFIC_COMPLETION_FUNCTIONS)
  rl_bind_key_if_unbound_in_map ('/', bash_complete_filename, emacs_meta_keymap);
  rl_bind_key_if_unbound_in_map ('/', bash_possible_filename_completions, emacs_ctlx_keymap);

  /* Have to jump through hoops here because there is a default binding for
     M-~ (rl_tilde_expand) */
  kseq[0] = '~';
  kseq[1] = '\0';
  func = rl_function_of_keyseq (kseq, emacs_meta_keymap, (int *)NULL);
  if (func == 0 || func == rl_tilde_expand)
    rl_bind_keyseq_in_map (kseq, bash_complete_username, emacs_meta_keymap);

  rl_bind_key_if_unbound_in_map ('~', bash_possible_username_completions, emacs_ctlx_keymap);

  rl_bind_key_if_unbound_in_map ('@', bash_complete_hostname, emacs_meta_keymap);
  rl_bind_key_if_unbound_in_map ('@', bash_possible_hostname_completions, emacs_ctlx_keymap);

  rl_bind_key_if_unbound_in_map ('$', bash_complete_variable, emacs_meta_keymap);
  rl_bind_key_if_unbound_in_map ('$', bash_possible_variable_completions, emacs_ctlx_keymap);

  rl_bind_key_if_unbound_in_map ('!', bash_complete_command, emacs_meta_keymap);
  rl_bind_key_if_unbound_in_map ('!', bash_possible_command_completions, emacs_ctlx_keymap);

  rl_bind_key_if_unbound_in_map ('g', bash_glob_complete_word, emacs_meta_keymap);
  rl_bind_key_if_unbound_in_map ('*', bash_glob_expand_word, emacs_ctlx_keymap);
  rl_bind_key_if_unbound_in_map ('g', bash_glob_list_expansions, emacs_ctlx_keymap);

#endif /* SPECIFIC_COMPLETION_FUNCTIONS */

  kseq[0] = TAB;
  kseq[1] = '\0';
  func = rl_function_of_keyseq (kseq, emacs_meta_keymap, (int *)NULL);
  if (func == 0 || func == rl_tab_insert)
    rl_bind_key_in_map (TAB, dynamic_complete_history, emacs_meta_keymap);

  /* Tell the completer that we want a crack first. */
  rl_attempted_completion_function = attempt_shell_completion;

  /* Tell the completer that we might want to follow symbolic links or
     do other expansion on directory names. */
  set_directory_hook ();

  rl_filename_rewrite_hook = bash_filename_rewrite_hook;

  rl_filename_stat_hook = bash_filename_stat_hook;

  /* Tell the filename completer we want a chance to ignore some names. */
  rl_ignore_some_completions_function = filename_completion_ignore;

  /* Bind C-xC-e to invoke emacs and run result as commands. */
  rl_bind_key_if_unbound_in_map (CTRL ('E'), emacs_edit_and_execute_command, emacs_ctlx_keymap);
#if defined (VI_MODE)
  rl_bind_key_if_unbound_in_map ('v', vi_edit_and_execute_command, vi_movement_keymap);
#  if defined (ALIAS)
  rl_bind_key_if_unbound_in_map ('@', posix_edit_macros, vi_movement_keymap);
#  endif

  rl_bind_key_in_map ('\\', bash_vi_complete, vi_movement_keymap);
  rl_bind_key_in_map ('*', bash_vi_complete, vi_movement_keymap);
  rl_bind_key_in_map ('=', bash_vi_complete, vi_movement_keymap);
#endif

  rl_completer_quote_characters = "'\"";

  /* This sets rl_completer_word_break_characters and rl_special_prefixes
     to the appropriate values, depending on whether or not hostname
     completion is enabled. */
  enable_hostname_completion (perform_hostname_completion);

  /* characters that need to be quoted when appearing in filenames. */
  rl_filename_quote_characters = default_filename_quote_characters;
  set_filename_bstab (rl_filename_quote_characters);

  rl_filename_quoting_function = bash_quote_filename;
  rl_filename_dequoting_function = bash_dequote_filename;
  rl_char_is_quoted_p = char_is_quoted;

  /* Add some default bindings for the "shellwords" functions, roughly
     parallelling the default word bindings in emacs mode. */
  rl_bind_key_if_unbound_in_map (CTRL('B'), bash_backward_shellword, emacs_meta_keymap);
  rl_bind_key_if_unbound_in_map (CTRL('D'), bash_kill_shellword, emacs_meta_keymap);
  rl_bind_key_if_unbound_in_map (CTRL('F'), bash_forward_shellword, emacs_meta_keymap);
  rl_bind_key_if_unbound_in_map (CTRL('T'), bash_transpose_shellwords, emacs_meta_keymap);

#if 0
  /* This is superfluous and makes it impossible to use tab completion in
     vi mode even when explicitly binding it in ~/.inputrc.  sv_strict_posix()
     should already have called posix_readline_initialize() when
     posixly_correct was set. */
  if (posixly_correct)
    posix_readline_initialize (1);
#endif

  bash_readline_initialized = 1;
}