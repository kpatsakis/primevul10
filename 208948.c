void generate_subdir_name(char *file_name, char *subdir, int size) {
   char fn[MAX_PATH_LENGTH], path[MAX_PATH_LENGTH];
   int year;

   // extract path from file_name
   strlcpy(path, file_name, size);
   if (strrchr(path, DIR_SEPARATOR))
      *(strrchr(path, DIR_SEPARATOR) + 1) = 0;

   // extract file name
   if (strrchr(file_name, DIR_SEPARATOR))
      strlcpy(fn, strrchr(file_name, DIR_SEPARATOR) + 1, sizeof(fn));
   else
      strlcpy(fn, file_name, sizeof(fn));

   // create subdir from name
   year = (fn[0] - '0') * 10 + (fn[1] - '0');
   // month = (fn[2]-'0')*10+(fn[3]-'0');
   if (year < 80)
      sprintf(subdir, "20%02d", year);
   else
      sprintf(subdir, "19%02d", year);

   strlcat(subdir, DIR_SEPARATOR_STR, size);
}