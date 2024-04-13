attlist_attach_file (mu_list_t *attlist_ptr,
		     int fd,
		     const char *realname,
		     const char *content_filename, const char *content_name,
		     const char *content_type, const char *encoding)
{
  int rc;
  struct stat st;
  mu_list_t list;
  mu_stream_t stream = NULL;
  char *id = NULL;
  mu_list_t attlist;

  if (fd >= 0)
    {
      rc = mu_fd_stream_create (&stream, NULL, fd, MU_STREAM_READ);
      if (rc)
	{
	  mu_error (_("can't open descriptor %d: %s"), fd, mu_strerror (rc));
	  return 1;
	}
      mu_asprintf (&id, "fd %d", fd);
      if (fd == 0)
	{
	  mu_stream_destroy (&mu_strin);
	  mu_nullstream_create (&mu_strin, MU_STREAM_READ);
	  mu_stream_ioctl (mu_strin, MU_IOCTL_NULLSTREAM,
			   MU_IOCTL_NULLSTREAM_SET_PATTERN, NULL);
	  util_do_command ("set nullbody nullbodymsg");
	}
    }
  else if (realname)
    {
      if (!content_filename)
	content_filename = realname;
      if (stat (realname, &st))
	{
	  if (errno == ENOENT)
	    {
	      mu_error (_("%s: file does not exist"), realname);
	      return 1;
	    }
	  else
	    {
	      mu_error (_("%s: cannot stat: %s"), realname,
			mu_strerror (errno));
	      return 1;
	    }
	}

      if (S_ISREG (st.st_mode))
	rc = mu_mapfile_stream_create (&stream, realname, MU_STREAM_READ);
      else if (S_ISFIFO (st.st_mode))
	rc = mu_file_stream_create (&stream, realname, MU_STREAM_READ);
      else
	{
	  mu_error (_("%s: not a regular file or FIFO"), realname);
	  return 1;
	}

      if (rc)
	{
	  mu_error (_("can't open file %s: %s"),
		    realname, mu_strerror (rc));
	  return 1;
	}
      mu_asprintf (&id, "\"%s\"", realname);
    }
  else
    abort ();
  
  if (!encoding)
    encoding = "base64";
  mu_filter_get_list (&list);
  rc = mu_list_locate (list, (void*) encoding, NULL);
  if (rc)
    {
      mu_error (_("unsupported encoding: %s"), encoding);
      free (id);
      mu_stream_destroy (&stream);
      return 1;
    }
  
  if (!*attlist_ptr)
    {
      attlist = attlist_new ();
      *attlist_ptr = attlist;
    }
  else
    attlist = *attlist_ptr;
    
  attlist_add (attlist, id, encoding, content_type,
	       content_name, content_filename,
	       stream, skip_empty_attachments, 0);
  if (stream)
    mu_stream_unref (stream);
  free (id);

  return 0;
}