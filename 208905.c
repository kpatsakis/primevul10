int restructure_dir(char *dir) {
   char *file_list;
   int n1, n2, index, status;
   char old_path[MAX_PATH_LENGTH], new_path[MAX_PATH_LENGTH],
           subdir[MAX_PATH_LENGTH];
   static int first = TRUE;

   /* go through all entry files */
   n1 = ss_file_find(dir, "??????a.log", &file_list);
   for (index = 0; index < n1; index++) {
      generate_subdir_name(file_list + index * MAX_PATH_LENGTH, subdir, sizeof(subdir));

      // create new subdir
      strlcpy(new_path, dir, MAX_PATH_LENGTH);
      strlcat(new_path, subdir, MAX_PATH_LENGTH);

#ifdef OS_WINNT
      status = mkdir(new_path);
#else
      status = mkdir(new_path, 0755);
#endif

      if (status == 0) {
         if (first) {
            eprintf("\nFound old directory structure. Creating subdirectories and moving files...\n");
            first = FALSE;
         }
         eprintf("Created directory \"%s\"\n", new_path);
      } else {
         if (errno != EEXIST) {
            eprintf("generate_subdir_name: %s\n", strerror(errno));
            eprintf("Cannot create directory \"%s\"\n", new_path);
         }
      }

      strlcpy(old_path, dir, sizeof(old_path));
      strlcat(old_path, file_list + index * MAX_PATH_LENGTH, sizeof(old_path));
      strlcpy(new_path, dir, sizeof(new_path));
      strlcat(new_path, subdir, sizeof(new_path));
      strlcat(new_path, file_list + index * MAX_PATH_LENGTH, sizeof(new_path));

      rename(old_path, new_path);
   }
   if (file_list)
      xfree(file_list);

   /* go through all attachment files */
   n2 = ss_file_find(dir, "??????_??????_*", &file_list);
   for (index = 0; index < n2; index++) {
      generate_subdir_name(file_list + index * MAX_PATH_LENGTH, subdir, sizeof(subdir));

      // create new subdir
      strlcpy(new_path, dir, MAX_PATH_LENGTH);
      strlcat(new_path, subdir, MAX_PATH_LENGTH);

#ifdef OS_WINNT
      status = mkdir(new_path);
#else
      status = mkdir(new_path, 0755);
#endif

      strlcpy(old_path, dir, sizeof(old_path));
      strlcat(old_path, file_list + index * MAX_PATH_LENGTH, sizeof(old_path));
      strlcpy(new_path, dir, sizeof(new_path));
      strlcat(new_path, subdir, sizeof(new_path));
      strlcat(new_path, file_list + index * MAX_PATH_LENGTH, sizeof(new_path));

      rename(old_path, new_path);
   }
   if (file_list)
      xfree(file_list);

   return n1 + n2;
}