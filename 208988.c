BOOL file_exist(char *file_name) {
   int fh;

   fh = open(file_name, O_RDONLY);
   if (fh < 0)
      return FALSE;

   close(fh);
   return TRUE;
}