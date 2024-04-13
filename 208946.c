int ss_file_find(const char *path, char *pattern, char **plist)
/********************************************************************
 Routine: ss_file_find

 Purpose: Return list of files matching 'pattern' from the 'path' location

 Input:
 char  *path             Name of a file in file system to check
 char  *pattern          pattern string (wildcard allowed)

 Output:
 char  **plist           pointer to the file list

 Function value:
 int                     Number of files matching request

 \********************************************************************/
{
#ifdef OS_UNIX
   DIR *dir_pointer;
   struct dirent *dp;
   int i;

   if ((dir_pointer = opendir(path)) == NULL)
      return 0;
   *plist = (char *) xmalloc(MAX_PATH_LENGTH);
   i = 0;
   for (dp = readdir(dir_pointer); dp != NULL; dp = readdir(dir_pointer)) {
      if (fnmatch1(pattern, dp->d_name) == 0) {
         *plist = (char *) xrealloc(*plist, (i + 1) * MAX_PATH_LENGTH);
         strncpy(*plist + (i * MAX_PATH_LENGTH), dp->d_name, strlen(dp->d_name));
         *(*plist + (i * MAX_PATH_LENGTH) + strlen(dp->d_name)) = '\0';
         i++;
      }
   }
   closedir(dir_pointer);
   return i;
#endif

#ifdef OS_WINNT
                                                                                                                           HANDLE pffile;
   LPWIN32_FIND_DATA lpfdata;
   char str[255];
   int i, first;

   strlcpy(str, path, sizeof(str));
   strlcat(str, "\\", sizeof(str));
   strlcat(str, pattern, sizeof(str));
   first = 1;
   i = 0;
   lpfdata = xmalloc(sizeof(WIN32_FIND_DATA));
   *plist = (char *) xmalloc(MAX_PATH_LENGTH);
   pffile = FindFirstFile(str, lpfdata);
   if (pffile == INVALID_HANDLE_VALUE)
      return 0;
   first = 0;
   *plist = (char *) xrealloc(*plist, (i + 1) * MAX_PATH_LENGTH);
   strncpy(*plist + (i * MAX_PATH_LENGTH), lpfdata->cFileName, strlen(lpfdata->cFileName));
   *(*plist + (i * MAX_PATH_LENGTH) + strlen(lpfdata->cFileName)) = '\0';
   i++;
   while (FindNextFile(pffile, lpfdata)) {
      *plist = (char *) xrealloc(*plist, (i + 1) * MAX_PATH_LENGTH);
      strncpy(*plist + (i * MAX_PATH_LENGTH), lpfdata->cFileName, strlen(lpfdata->cFileName));
      *(*plist + (i * MAX_PATH_LENGTH) + strlen(lpfdata->cFileName)) = '\0';
      i++;
   }
   xfree(lpfdata);
   return i;
#endif
}