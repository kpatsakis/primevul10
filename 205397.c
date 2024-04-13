int compare_files2(File fd1, const char* filename2)
{
  int error= RESULT_OK;
  File fd2;
  size_t fd1_length, fd2_length;
  DYNAMIC_STRING fd1_result, fd2_result;

  if ((fd2= my_open(filename2, O_RDONLY, MYF(0))) < 0)
  {
    my_close(fd1, MYF(0));
    die("Failed to open second file: '%s'", filename2);
  }

  fd1_length= (size_t) my_seek(fd1, 0, SEEK_END, MYF(0));
  fd2_length= (size_t) my_seek(fd2, 0, SEEK_END, MYF(0));

  if (init_dynamic_string(&fd1_result, 0, fd1_length, 0) ||
      init_dynamic_string(&fd2_result, 0, fd2_length, 0))
    die("Out of memory when allocating data for result");

  fd1_result.length= fd1_length;
  fd2_result.length= fd2_length;

  (void) my_seek(fd1, 0, SEEK_SET, MYF(0));
  (void) my_seek(fd2, 0, SEEK_SET, MYF(0));
  if (my_read(fd1, (uchar*) fd1_result.str, fd1_length, MYF(MY_WME | MY_NABP)))
    die("Error when reading data from result file");
  if (my_read(fd2, (uchar*) fd2_result.str, fd2_length, MYF(MY_WME | MY_NABP)))
    die("Error when reading data from result file");

  if (global_subst &&
      (fd1_length != fd2_length ||
       memcmp(fd1_result.str, fd2_result.str, fd1_length)))
  {
    /**
       @todo MARIA_HACK
       This serves for when a test is run with --default-storage-engine=X
       where X is not MyISAM: tests using SHOW CREATE TABLE will always fail
       because SHOW CREATE TABLE prints X instead of MyISAM. With
       --global-subst=X,MyISAM , such trivial differences are eliminated and
       test may be reported as passing.
       --global-subst is only a quick way to run a lot of existing tests
       with Maria and find bugs; it is not good enough for reaching the main
       trees when Maria is merged into them.
         --global-subst should be removed.
    */
    uint global_subst_from_len= strlen(global_subst_from);
    uint global_subst_to_len=   strlen(global_subst_to);
    while (replace(&fd1_result,
                   global_subst_from, global_subst_from_len,
                   global_subst_to,   global_subst_to_len) == 0)
      /* do nothing */ ;
    /* let's compare again to see if it is ok now */
  }

  if (fd1_result.length != fd2_result.length)
    error= RESULT_LENGTH_MISMATCH;
  else if ((memcmp(fd1_result.str, fd2_result.str, fd1_result.length)))
    error= RESULT_CONTENT_MISMATCH;

  my_close(fd2, MYF(0));
  dynstr_free(&fd1_result);
  dynstr_free(&fd2_result);

  return error;
}