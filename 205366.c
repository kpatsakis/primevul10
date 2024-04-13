int dyn_string_cmp(DYNAMIC_STRING* ds, const char *fname)
{
  int error;
  File fd;
  char temp_file_path[FN_REFLEN];

  DBUG_ENTER("dyn_string_cmp");
  DBUG_PRINT("enter", ("fname: %s", fname));

  if ((fd= create_temp_file(temp_file_path, TMPDIR,
                            "tmp", O_CREAT | O_SHARE | O_RDWR,
                            MYF(MY_WME))) < 0)
    die("Failed to create temporary file for ds");

  /* Write ds to temporary file and set file pos to beginning*/
  if (my_write(fd, (uchar *) ds->str, ds->length,
               MYF(MY_FNABP | MY_WME)) ||
      my_seek(fd, 0, SEEK_SET, MYF(0)) == MY_FILEPOS_ERROR)
  {
    my_close(fd, MYF(0));
    /* Remove the temporary file */
    my_delete(temp_file_path, MYF(MY_WME));
    die("Failed to write file '%s'", temp_file_path);
  }

  error= compare_files2(fd, fname);

  my_close(fd, MYF(0));
  /* Remove the temporary file */
  my_delete(temp_file_path, MYF(MY_WME));

  DBUG_RETURN(error);
}