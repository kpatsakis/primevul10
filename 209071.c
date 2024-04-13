int my_shell(char *cmd, char *result, int size) {
#ifdef OS_WINNT

                                                                                                                           HANDLE hChildStdinRd, hChildStdinWr, hChildStdinWrDup,
       hChildStdoutRd, hChildStdoutWr, hChildStderrRd, hChildStderrWr, hSaveStdin, hSaveStdout, hSaveStderr;

   SECURITY_ATTRIBUTES saAttr;
   PROCESS_INFORMATION piProcInfo;
   STARTUPINFO siStartInfo;
   char buffer[10000];
   DWORD dwRead, dwAvail, i;

   /* Set the bInheritHandle flag so pipe handles are inherited. */
   saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
   saAttr.bInheritHandle = TRUE;
   saAttr.lpSecurityDescriptor = NULL;

   /* Save the handle to the current STDOUT. */
   hSaveStdout = GetStdHandle(STD_OUTPUT_HANDLE);

   /* Create a pipe for the child's STDOUT. */
   if (!CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0))
      return 0;

   /* Set a write handle to the pipe to be STDOUT. */
   if (!SetStdHandle(STD_OUTPUT_HANDLE, hChildStdoutWr))
      return 0;

   /* Save the handle to the current STDERR. */
   hSaveStderr = GetStdHandle(STD_ERROR_HANDLE);

   /* Create a pipe for the child's STDERR. */
   if (!CreatePipe(&hChildStderrRd, &hChildStderrWr, &saAttr, 0))
      return 0;

   /* Set a read handle to the pipe to be STDERR. */
   if (!SetStdHandle(STD_ERROR_HANDLE, hChildStderrWr))
      return 0;

   /* Save the handle to the current STDIN. */
   hSaveStdin = GetStdHandle(STD_INPUT_HANDLE);

   /* Create a pipe for the child's STDIN. */
   if (!CreatePipe(&hChildStdinRd, &hChildStdinWr, &saAttr, 0))
      return 0;

   /* Set a read handle to the pipe to be STDIN. */
   if (!SetStdHandle(STD_INPUT_HANDLE, hChildStdinRd))
      return 0;

   /* Duplicate the write handle to the pipe so it is not inherited. */
   if (!DuplicateHandle(GetCurrentProcess(), hChildStdinWr, GetCurrentProcess(), &hChildStdinWrDup, 0, FALSE,   /* not inherited */
                        DUPLICATE_SAME_ACCESS))
      return 0;

   CloseHandle(hChildStdinWr);

   /* Now create the child process. */
   memset(&siStartInfo, 0, sizeof(siStartInfo));
   siStartInfo.cb = sizeof(STARTUPINFO);
   siStartInfo.lpReserved = NULL;
   siStartInfo.lpReserved2 = NULL;
   siStartInfo.cbReserved2 = 0;
   siStartInfo.lpDesktop = NULL;
   siStartInfo.dwFlags = 0;

   /* command to execute */
   sprintf(buffer, "cmd /q /c %s", cmd);

   if (!CreateProcess(NULL, buffer,     /* command line */
                      NULL,     /* process security attributes */
                      NULL,     /* primary thread security attributes */
                      TRUE,     /* handles are inherited */
                      0,        /* creation flags */
                      NULL,     /* use parent's environment */
                      NULL,     /* use parent's current directory */
                      &siStartInfo,     /* STARTUPINFO pointer */
                      &piProcInfo))     /* receives PROCESS_INFORMATION */
      return 0;

   /* After process creation, restore the saved STDIN and STDOUT. */
   SetStdHandle(STD_INPUT_HANDLE, hSaveStdin);
   SetStdHandle(STD_OUTPUT_HANDLE, hSaveStdout);
   SetStdHandle(STD_ERROR_HANDLE, hSaveStderr);

   memset(result, 0, size);

   do {
      /* query stdout */
      do {
         if (!PeekNamedPipe(hChildStdoutRd, buffer, 256, &dwRead, &dwAvail, NULL))
            break;
         if (dwRead > 0) {
            ReadFile(hChildStdoutRd, buffer, 256, &dwRead, NULL);
            buffer[dwRead] = 0;
            strlcat(result, buffer, size);
         }
      } while (dwAvail > 0);

      /* query stderr */
      do {
         if (!PeekNamedPipe(hChildStderrRd, buffer, 256, &dwRead, &dwAvail, NULL))
            break;
         if (dwRead > 0) {
            ReadFile(hChildStderrRd, buffer, 256, &dwRead, NULL);
            buffer[dwRead] = 0;
            strlcat(result, buffer, size);
         }
      } while (dwAvail > 0);

      /* check if subprocess still alive */
      if (!GetExitCodeProcess(piProcInfo.hProcess, &i))
         break;
      if (i != STILL_ACTIVE)
         break;

      /* give some CPU to subprocess */
      Sleep(10);

   } while (TRUE);

   CloseHandle(hChildStdinWrDup);
   CloseHandle(hChildStdinRd);
   CloseHandle(hChildStderrRd);
   CloseHandle(hChildStdoutRd);

   /* strip trailing CR/LF */
   while (strlen(result) > 0 && (result[strlen(result) - 1] == '\r' || result[strlen(result) - 1] == '\n'))
      result[strlen(result) - 1] = 0;

   return 1;

#endif                          /* OS_WINNT */

#ifdef OS_UNIX
   pid_t child_pid;
   int fh, status, wait_status;
   char str[1024];
   char tmp_filename[1024];

   strlcpy(tmp_filename, "/tmp/elog_XXXXXX", sizeof(tmp_filename));
   fh = mkstemp(tmp_filename);
   if (fh == 0) {
      eprintf("Error getting TMP file name.\n");
      return 0;
   }
   close(fh);

   if ((child_pid = fork()) < 0)
      return 0;
   else if (child_pid > 0) {
      /* parent process waits for child */
      do {
         wait_status = waitpid(child_pid, &status, 0);
      } while (wait_status == -1 && errno == EINTR);

      /* read back result */
      memset(result, 0, size);
      fh = open(tmp_filename, O_RDONLY);
      if (fh > 0) {
         read(fh, result, size - 1);
         close(fh);
      }

      /* remove temporary file */
      remove(tmp_filename);

      /* strip trailing CR/LF */
      while (strlen(result) > 0 && (result[strlen(result) - 1] == '\r' || result[strlen(result) - 1] == '\n'))
         result[strlen(result) - 1] = 0;

   } else {
      /* child process */

      /* restore original UID/GID */
      if (setregid(-1, orig_gid) < 0 || setreuid(-1, orig_uid) < 0)
         eprintf("Cannot restore original GID/UID.\n");

      /* give up root privilege permanently */
      if (geteuid() == 0) {
         if (!getcfg("global", "Grp", str, sizeof(str)) || setgroup(str) < 0) {
            eprintf("Falling back to default group \"elog\"\n");
            if (setgroup("elog") < 0) {
               eprintf("Falling back to default group \"%s\"\n", DEFAULT_GROUP);
               if (setgroup(DEFAULT_GROUP) < 0) {
                  eprintf("Refuse to run as setgid root.\n");
                  eprintf("Please consider to define a Grp statement in configuration file\n");
                  exit(EXIT_FAILURE);
               }
            }
         } else if (get_verbose() >= VERBOSE_INFO)
            eprintf("Falling back to group \"%s\"\n", str);

         if (!getcfg("global", "Usr", str, sizeof(str)) || setuser(str) < 0) {
            eprintf("Falling back to default user \"elog\"\n");
            if (setuser("elog") < 0) {
               eprintf("Falling back to default user \"%s\"\n", DEFAULT_USER);
               if (setuser(DEFAULT_USER) < 0) {
                  eprintf("Refuse to run as setuid root.\n");
                  eprintf("Please consider to define a Usr statement in configuration file\n");
                  exit(EXIT_FAILURE);
               }
            }
         } else if (get_verbose() >= VERBOSE_INFO)
            eprintf("Falling back to user \"%s\"\n", str);
      }

      /* execute shell with redirection to /tmp/elog-shell */
      sprintf(str, "/bin/sh -c \"%s\" > %s 2>&1", cmd, tmp_filename);

      if (get_verbose() >= VERBOSE_INFO) {
         efputs("Going to execute: ");
         efputs(str);
         efputs("\n");
      }

      system(str);
      _exit(0);
   }

   return 1;

#endif                          /* OS_UNIX */
}