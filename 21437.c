glob_in_dir (const char *pattern, const char *directory, int flags,
             int (*errfunc) (const char *, int),
             glob_t *pglob, size_t alloca_used)
{
  size_t dirlen = strlen (directory);
  void *stream = NULL;
# define GLOBNAMES_MEMBERS(nnames) \
    struct globnames *next; size_t count; char *name[nnames];
  struct globnames { GLOBNAMES_MEMBERS (FLEXIBLE_ARRAY_MEMBER) };
  struct { GLOBNAMES_MEMBERS (64) } init_names_buf;
  struct globnames *init_names = (struct globnames *) &init_names_buf;
  struct globnames *names = init_names;
  struct globnames *names_alloca = init_names;
  size_t nfound = 0;
  size_t cur = 0;
  int meta;
  int save;
  int result;

  alloca_used += sizeof init_names_buf;

  init_names->next = NULL;
  init_names->count = ((sizeof init_names_buf
                        - offsetof (struct globnames, name))
                       / sizeof init_names->name[0]);

  meta = __glob_pattern_type (pattern, !(flags & GLOB_NOESCAPE));
  if (meta == GLOBPAT_NONE && (flags & (GLOB_NOCHECK|GLOB_NOMAGIC)))
    {
      /* We need not do any tests.  The PATTERN contains no meta
         characters and we must not return an error therefore the
         result will always contain exactly one name.  */
      flags |= GLOB_NOCHECK;
    }
  else if (meta == GLOBPAT_NONE)
    {
      union
      {
        struct stat st;
        struct_stat64 st64;
      } ust;
      size_t patlen = strlen (pattern);
      size_t fullsize;
      bool alloca_fullname
        = (! size_add_wrapv (dirlen + 1, patlen + 1, &fullsize)
           && glob_use_alloca (alloca_used, fullsize));
      char *fullname;
      if (alloca_fullname)
        fullname = alloca_account (fullsize, alloca_used);
      else
        {
          fullname = malloc (fullsize);
          if (fullname == NULL)
            return GLOB_NOSPACE;
        }

      mempcpy (mempcpy (mempcpy (fullname, directory, dirlen),
                        "/", 1),
               pattern, patlen + 1);
      if (((__builtin_expect (flags & GLOB_ALTDIRFUNC, 0)
            ? (*pglob->gl_lstat) (fullname, &ust.st)
            : __lstat64 (fullname, &ust.st64))
           == 0)
          || errno == EOVERFLOW)
        /* We found this file to be existing.  Now tell the rest
           of the function to copy this name into the result.  */
        flags |= GLOB_NOCHECK;

      if (__glibc_unlikely (!alloca_fullname))
        free (fullname);
    }
  else
    {
      stream = (__builtin_expect (flags & GLOB_ALTDIRFUNC, 0)
                ? (*pglob->gl_opendir) (directory)
                : opendir (directory));
      if (stream == NULL)
        {
          if (errno != ENOTDIR
              && ((errfunc != NULL && (*errfunc) (directory, errno))
                  || (flags & GLOB_ERR)))
            return GLOB_ABORTED;
        }
      else
        {
          int fnm_flags = ((!(flags & GLOB_PERIOD) ? FNM_PERIOD : 0)
                           | ((flags & GLOB_NOESCAPE) ? FNM_NOESCAPE : 0));
          flags |= GLOB_MAGCHAR;

          while (1)
            {
              struct readdir_result d;
              {
                if (__builtin_expect (flags & GLOB_ALTDIRFUNC, 0))
                  d = convert_dirent (GL_READDIR (pglob, stream));
                else
                  {
#ifdef COMPILE_GLOB64
                    d = convert_dirent (__readdir (stream));
#else
                    d = convert_dirent64 (__readdir64 (stream));
#endif
                  }
              }
              if (d.name == NULL)
                break;

              /* If we shall match only directories use the information
                 provided by the dirent call if possible.  */
              if (flags & GLOB_ONLYDIR)
                switch (readdir_result_type (d))
                  {
                  case DT_DIR: case DT_LNK: case DT_UNKNOWN: break;
                  default: continue;
                  }

              if (fnmatch (pattern, d.name, fnm_flags) == 0)
                {
                  if (cur == names->count)
                    {
                      struct globnames *newnames;
                      size_t count = names->count * 2;
                      size_t nameoff = offsetof (struct globnames, name);
                      size_t size = FLEXSIZEOF (struct globnames, name,
                                                count * sizeof (char *));
                      if ((SIZE_MAX - nameoff) / 2 / sizeof (char *)
                          < names->count)
                        goto memory_error;
                      if (glob_use_alloca (alloca_used, size))
                        newnames = names_alloca
                          = alloca_account (size, alloca_used);
                      else if ((newnames = malloc (size))
                               == NULL)
                        goto memory_error;
                      newnames->count = count;
                      newnames->next = names;
                      names = newnames;
                      cur = 0;
                    }
                  names->name[cur] = strdup (d.name);
                  if (names->name[cur] == NULL)
                    goto memory_error;
                  ++cur;
                  ++nfound;
                  if (SIZE_MAX - pglob->gl_offs <= nfound)
                    goto memory_error;
                }
            }
        }
    }

  if (nfound == 0 && (flags & GLOB_NOCHECK))
    {
      size_t len = strlen (pattern);
      nfound = 1;
      names->name[cur] = malloc (len + 1);
      if (names->name[cur] == NULL)
        goto memory_error;
      *((char *) mempcpy (names->name[cur++], pattern, len)) = '\0';
    }

  result = GLOB_NOMATCH;
  if (nfound != 0)
    {
      char **new_gl_pathv;
      result = 0;

      if (SIZE_MAX / sizeof (char *) - pglob->gl_pathc
          < pglob->gl_offs + nfound + 1)
        goto memory_error;

      new_gl_pathv
        = realloc (pglob->gl_pathv,
                   (pglob->gl_pathc + pglob->gl_offs + nfound + 1)
                    * sizeof (char *));

      if (new_gl_pathv == NULL)
        {
        memory_error:
          while (1)
            {
              struct globnames *old = names;
              for (size_t i = 0; i < cur; ++i)
                free (names->name[i]);
              names = names->next;
              /* NB: we will not leak memory here if we exit without
                 freeing the current block assigned to OLD.  At least
                 the very first block is always allocated on the stack
                 and this is the block assigned to OLD here.  */
              if (names == NULL)
                {
                  assert (old == init_names);
                  break;
                }
              cur = names->count;
              if (old == names_alloca)
                names_alloca = names;
              else
                free (old);
            }
          result = GLOB_NOSPACE;
        }
      else
        {
          while (1)
            {
              struct globnames *old = names;
              for (size_t i = 0; i < cur; ++i)
                new_gl_pathv[pglob->gl_offs + pglob->gl_pathc++]
                  = names->name[i];
              names = names->next;
              /* NB: we will not leak memory here if we exit without
                 freeing the current block assigned to OLD.  At least
                 the very first block is always allocated on the stack
                 and this is the block assigned to OLD here.  */
              if (names == NULL)
                {
                  assert (old == init_names);
                  break;
                }
              cur = names->count;
              if (old == names_alloca)
                names_alloca = names;
              else
                free (old);
            }

          pglob->gl_pathv = new_gl_pathv;

          pglob->gl_pathv[pglob->gl_offs + pglob->gl_pathc] = NULL;

          pglob->gl_flags = flags;
        }
    }

  if (stream != NULL)
    {
      save = errno;
      if (__glibc_unlikely (flags & GLOB_ALTDIRFUNC))
        (*pglob->gl_closedir) (stream);
      else
        closedir (stream);
      __set_errno (save);
    }

  return result;
}