int ofx_proc_file(LibofxContextPtr ctx, const char * p_filename)
{
  LibofxContext *libofx_context;
  bool ofx_start = false;
  bool ofx_end = false;
  bool file_is_xml = false;

  ifstream input_file;
  ofstream tmp_file;
  char buffer[READ_BUFFER_SIZE];
  char *iconv_buffer;
  string s_buffer;
  char *filenames[3];
  char tmp_filename[256];
  int tmp_file_fd;
#ifdef HAVE_ICONV
  iconv_t conversion_descriptor;
#endif
  libofx_context = (LibofxContext*)ctx;

  if (p_filename != NULL && strcmp(p_filename, "") != 0)
  {
    message_out(DEBUG, string("ofx_proc_file():Opening file: ") + p_filename);

    input_file.open(p_filename);
    if (!input_file)
    {
      message_out(ERROR, "ofx_proc_file():Unable to open the input file " + string(p_filename));
    }

    mkTempFileName("libofxtmpXXXXXX", tmp_filename, sizeof(tmp_filename));

    message_out(DEBUG, "ofx_proc_file(): Creating temp file: " + string(tmp_filename));
    tmp_file_fd = mkstemp(tmp_filename);
    if (tmp_file_fd)
    {
      tmp_file.open(tmp_filename);
      if (!tmp_file)
      {
        message_out(ERROR, "ofx_proc_file():Unable to open the created temp file " + string(tmp_filename));
        return -1;
      }
    }
    else
    {
      message_out(ERROR, "ofx_proc_file():Unable to create a temp file at " + string(tmp_filename));
      return -1;
    }

    if (input_file && tmp_file)
    {
      int header_separator_idx;
      string header_name;
      string header_value;
      string ofx_encoding;
      string ofx_charset;
      do
      {
        s_buffer.clear();
        bool end_of_line = false;
        do
        {
          input_file.get(buffer, sizeof(buffer), '\n');
          //cout<< "got: \"" << buffer<<"\"\n";
          s_buffer.append(buffer);

          // Watch out: If input_file is in eof(), any subsequent read or
          // peek() will fail and we must exit this loop.
          if (input_file.eof())
            break;

          //cout<<"input_file.gcount(): "<<input_file.gcount()<< " s_buffer.size=" << s_buffer.size()<<" sizeof(buffer): "<<sizeof(buffer) << " peek=\"" << int(input_file.peek()) << "\"" <<endl;
          if (input_file.fail()) // If no characters were extracted above, the failbit is set.
          {
            // No characters extracted means that we've reached the newline
            // delimiter (because we already checked for EOF). We will check
            // for and remove that newline in the next if-clause, but must
            // remove the failbit so that peek() will work again.
            input_file.clear();
          }

          // Is the next character really the newline?
          if (input_file.peek() == '\n')
          {
            // Yes. Then discard that newline character from the stream and
            // append it manually to the output string.
            input_file.get();
            s_buffer.append("\n");
            end_of_line = true; // We found the end-of-line.
          }
        }
        // Continue reading as long as we're not at EOF *and* we've not yet
        // reached an end-of-line.
        while (!input_file.eof() && !end_of_line);

        if (ofx_start == false && (s_buffer.find("<?xml") != string::npos))
        {
          message_out(DEBUG, "ofx_proc_file(): File is an actual XML file, iconv conversion will be skipped.");
          file_is_xml = true;
        }

        int ofx_start_idx;
        if (ofx_start == false &&
            (
              (libofx_context->currentFileType() == OFX &&
               ((ofx_start_idx = s_buffer.find("<OFX>")) !=
                string::npos || (ofx_start_idx = s_buffer.find("<ofx>")) != string::npos))
              || (libofx_context->currentFileType() == OFC &&
                  ((ofx_start_idx = s_buffer.find("<OFC>")) != string::npos ||
                   (ofx_start_idx = s_buffer.find("<ofc>")) != string::npos))
            )
           )
        {
          ofx_start = true;
          if (file_is_xml == false)
          {
            s_buffer.erase(0, ofx_start_idx); //Fix for really broken files that don't have a newline after the header.
          }
          message_out(DEBUG, "ofx_proc_file():<OFX> or <OFC> has been found");

          if (file_is_xml == true)
          {
            static char sp_charset_fixed[] = "SP_CHARSET_FIXED=1";
            if (putenv(sp_charset_fixed) != 0)
            {
              message_out(ERROR, "ofx_proc_file(): putenv failed");
            }
            /* Normally the following would be "xml".
             * Unfortunately, opensp's generic api will garble UTF-8 if this is
             * set to xml.  So we set any single byte encoding to avoid messing
             * up UTF-8.  Unfortunately this means that non-UTF-8 files will not
             * get properly translated.  We'd need to manually detect the
             * encoding in the XML header and convert the xml with iconv like we
             * do for SGML to work around the problem.  Most unfortunate. */
            static char sp_encoding[] = "SP_ENCODING=ms-dos";
            if (putenv(sp_encoding) != 0)
            {
              message_out(ERROR, "ofx_proc_file(): putenv failed");
            }
          }
          else
          {
            static char sp_charset_fixed[] = "SP_CHARSET_FIXED=1";
            if (putenv(sp_charset_fixed) != 0)
            {
              message_out(ERROR, "ofx_proc_file(): putenv failed");
            }
            static char sp_encoding[] = "SP_ENCODING=ms-dos"; //Any single byte encoding will do, we don't want opensp messing up UTF-8;
            if (putenv(sp_encoding) != 0)
            {
              message_out(ERROR, "ofx_proc_file(): putenv failed");
            }
#ifdef HAVE_ICONV
            string fromcode;
            string tocode;
            if (ofx_encoding.compare("USASCII") == 0)
            {
              if (ofx_charset.compare("ISO-8859-1") == 0 || ofx_charset.compare("8859-1") == 0)
              {
                //Only "ISO-8859-1" is actually a legal value, but since the banks follows the spec SO well...
                fromcode = "ISO-8859-1";
              }
              else if (ofx_charset.compare("1252") == 0 || ofx_charset.compare("CP1252") == 0)
              {
                //Only "1252" is actually a legal value, but since the banks follows the spec SO well...
                fromcode = "CP1252";
              }
              else if (ofx_charset.compare("NONE") == 0)
              {
                fromcode = LIBOFX_DEFAULT_INPUT_ENCODING;
              }
              else
              {
                fromcode = LIBOFX_DEFAULT_INPUT_ENCODING;
              }
            }
            else if (ofx_encoding.compare("UTF-8") == 0 || ofx_encoding.compare("UNICODE") == 0)
            {
              //While "UNICODE" isn't a legal value, some cyrilic files do specify it as such...
              fromcode = "UTF-8";
            }
            else
            {
              fromcode = LIBOFX_DEFAULT_INPUT_ENCODING;
            }
            tocode = LIBOFX_DEFAULT_OUTPUT_ENCODING;
            message_out(DEBUG, "ofx_proc_file(): Setting up iconv for fromcode: " + fromcode + ", tocode: " + tocode);
            conversion_descriptor = iconv_open (tocode.c_str(), fromcode.c_str());
#endif
          }
        }
        else
        {
          //We are still in the headers
          if ((header_separator_idx = s_buffer.find(':')) != string::npos)
          {
            //Header processing
            header_name.assign(s_buffer.substr(0, header_separator_idx));
            header_value.assign(s_buffer.substr(header_separator_idx + 1));
            while ( header_value[header_value.length() -1 ] == '\n' ||
                    header_value[header_value.length() -1 ] == '\r' )
              header_value.erase(header_value.length() - 1);
            message_out(DEBUG, "ofx_proc_file():Header: " + header_name + " with value: " + header_value + " has been found");
            if (header_name.compare("ENCODING") == 0)
            {
              ofx_encoding.assign(header_value);
            }
            if (header_name.compare("CHARSET") == 0)
            {
              ofx_charset.assign(header_value);
            }
          }
        }

        if (file_is_xml == true || (ofx_start == true && ofx_end == false))
        {
          if (ofx_start == true)
          {
            /* The above test won't help us if the <OFX> tag is on the same line
             * as the xml header, but as opensp can't be used to parse it anyway
             * this isn't a great loss for now.
             */
            s_buffer = sanitize_proprietary_tags(s_buffer);
          }
          //cout<< s_buffer<<"\n";
          if (file_is_xml == false)
          {
#ifdef HAVE_ICONV
            size_t inbytesleft = strlen(s_buffer.c_str());
            size_t outbytesleft = inbytesleft * 2 - 1;
            iconv_buffer = (char*) malloc (inbytesleft * 2);
            memset(iconv_buffer, 0, inbytesleft * 2);
#if defined(OS_WIN32) || defined(__sun) || defined(__NetBSD__)
            const char * inchar = (const char *)s_buffer.c_str();
#else
            char * inchar = (char *)s_buffer.c_str();
#endif
            char * outchar = iconv_buffer;
            int iconv_retval = iconv (conversion_descriptor,
                                      &inchar, &inbytesleft,
                                      &outchar, &outbytesleft);
            if (iconv_retval == -1)
            {
              message_out(ERROR, "ofx_proc_file(): Conversion error");
            }
            s_buffer = iconv_buffer;
            free (iconv_buffer);
#endif
          }
          //cout << s_buffer << "\n";
          tmp_file.write(s_buffer.c_str(), s_buffer.length());
        }

        if (ofx_start == true &&
            (
              (libofx_context->currentFileType() == OFX &&
               ((ofx_start_idx = s_buffer.find("</OFX>")) != string::npos ||
                (ofx_start_idx = s_buffer.find("</ofx>")) != string::npos))
              || (libofx_context->currentFileType() == OFC &&
                  ((ofx_start_idx = s_buffer.find("</OFC>")) != string::npos ||
                   (ofx_start_idx = s_buffer.find("</ofc>")) != string::npos))
            )
           )
        {
          ofx_end = true;
          message_out(DEBUG, "ofx_proc_file():</OFX> or </OFC>  has been found");
        }

      }
      while (!input_file.eof() && !input_file.bad());
    }
    input_file.close();
    tmp_file.close();
#ifdef HAVE_ICONV
    if (file_is_xml == false)
    {
      iconv_close(conversion_descriptor);
    }
#endif
    char filename_openspdtd[255];
    char filename_dtd[255];
    char filename_ofx[255];
    strncpy(filename_openspdtd, find_dtd(ctx, OPENSPDCL_FILENAME).c_str(), 255); //The opensp sgml dtd file
    if (libofx_context->currentFileType() == OFX)
    {
      strncpy(filename_dtd, find_dtd(ctx, OFX160DTD_FILENAME).c_str(), 255); //The ofx dtd file
    }
    else if (libofx_context->currentFileType() == OFC)
    {
      strncpy(filename_dtd, find_dtd(ctx, OFCDTD_FILENAME).c_str(), 255); //The ofc dtd file
    }
    else
    {
      message_out(ERROR, string("ofx_proc_file(): Error unknown file format for the OFX parser"));
    }

    if ((string)filename_dtd != "" && (string)filename_openspdtd != "")
    {
      strncpy(filename_ofx, tmp_filename, 255); //The processed ofx file
      filenames[0] = filename_openspdtd;
      filenames[1] = filename_dtd;
      filenames[2] = filename_ofx;
      if (libofx_context->currentFileType() == OFX)
      {
        ofx_proc_sgml(libofx_context, 3, filenames);
      }
      else if (libofx_context->currentFileType() == OFC)
      {
        ofc_proc_sgml(libofx_context, 3, filenames);
      }
      else
      {
        message_out(ERROR, string("ofx_proc_file(): Error unknown file format for the OFX parser"));
      }
      if (remove(tmp_filename) != 0)
      {
        message_out(ERROR, "ofx_proc_file(): Error deleting temporary file " + string(tmp_filename));
      }
    }
    else
    {
      message_out(ERROR, "ofx_proc_file(): FATAL: Missing DTD, aborting");
    }
  }
  else
  {
    message_out(ERROR, "ofx_proc_file():No input file specified");
  }
  return 0;
}