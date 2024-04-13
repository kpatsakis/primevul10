int exist_file(char *file_name) {
   int fh;

   fh = open(file_name, O_RDONLY | O_BINARY);
   if (fh > 0) {
      close(fh);
      return 1;
   }
   return 0;
}