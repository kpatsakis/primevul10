void open_file(const char *name)
{
  char buff[FN_REFLEN];
  size_t length;
  const char *curname= cur_file->file_name;
  DBUG_ENTER("open_file");
  DBUG_PRINT("enter", ("name: %s", name));

  if (cur_file == file_stack_end)
    die("Source directives are nesting too deep");

  if (test_if_hard_path(name))
  {
    if (open_and_set_current(name))
      DBUG_VOID_RETURN;
  }
  else
  {
    /*
      if overlay-dir is specified, and the file is located somewhere
      under overlay-dir or under suite-dir, the search works as follows:

      0.let suffix be current file dirname relative to siute-dir or overlay-dir
      1.try in overlay-dir/suffix
      2.try in suite-dir/suffix
      3.try in overlay-dir
      4.try in suite-dir
      5.try in basedir

        consider an example: 'rty' overlay of the 'qwe' suite,
        file qwe/include/some.inc contains the line
          --source thing.inc
        we look for it in this order:
        0.suffix is "include/"
        1.try in rty/include/thing.inc
        2.try in qwe/include/thing.inc
        3.try in try/thing.inc             | this is useful when t/a.test has
        4.try in qwe/thing.inc             | source include/b.inc;
        5.try in mysql-test/include/thing.inc

      otherwise the search is as follows
      1.try in current file dirname
      3.try in overlay-dir (if any)
      4.try in suite-dir
      5.try in basedir
    */

#ifdef __WIN__
    fix_win_paths(curname, sizeof(curname));
#endif

    bool in_overlay= opt_overlay_dir &&
                     !strncmp(curname, opt_overlay_dir, overlay_dir_len);
    bool in_suiteir= opt_overlay_dir && !in_overlay &&
                     !strncmp(curname, opt_suite_dir, suite_dir_len);
    if (in_overlay || in_suiteir)
    {
      size_t prefix_len = in_overlay ? overlay_dir_len : suite_dir_len;
      char buf2[FN_REFLEN], *suffix= buf2 + prefix_len;
      dirname_part(buf2, curname, &length);

      /* 1. first we look in the overlay dir */
      strxnmov(buff, sizeof(buff), opt_overlay_dir, suffix, name, NullS);

      /*
        Overlayed rty/include/thing.inc can contain the line
        --source thing.inc
        which would mean to include qwe/include/thing.inc.
        But it looks like including "itself", so don't try to open the file,
        if buff contains the same file name as curname.
      */
      if (strcmp(buff, curname) && open_and_set_current(buff))
        DBUG_VOID_RETURN;

      /* 2. if that failed, we look in the suite dir */
      strxnmov(buff, sizeof(buff), opt_suite_dir, suffix, name, NullS);

      /* buff can not be equal to curname, as a file can never include itself */
      if (open_and_set_current(buff))
        DBUG_VOID_RETURN;
    }
    else
    {
      /* 1. try in current file dirname */
      dirname_part(buff, curname, &length);
      strxnmov(buff, sizeof(buff), buff, name, NullS);
      if (open_and_set_current(buff))
        DBUG_VOID_RETURN;
    }

    /* 3. now, look in the overlay dir */
    if (opt_overlay_dir)
    {
      strxmov(buff, opt_overlay_dir, name, NullS);
      if (open_and_set_current(buff))
        DBUG_VOID_RETURN;
    }

    /* 4. if that failed - look in the suite dir */
    strxmov(buff, opt_suite_dir, name, NullS);
    if (open_and_set_current(buff))
      DBUG_VOID_RETURN;
    
    /* 5. the last resort - look in the base dir */
    strxnmov(buff, sizeof(buff), opt_basedir, name, NullS);
    if (open_and_set_current(buff))
      DBUG_VOID_RETURN;
  }

  die("Could not open '%s' for reading, errno: %d", name, errno);
  DBUG_VOID_RETURN;
}