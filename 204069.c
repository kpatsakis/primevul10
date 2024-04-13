read_response_body (struct http_stat *hs, int sock, FILE *fp, wgint contlen,
                    wgint contrange, bool chunked_transfer_encoding,
                    char *url, char *warc_timestamp_str, char *warc_request_uuid,
                    ip_address *warc_ip, char *type, int statcode, char *head)
{
  int warc_payload_offset = 0;
  FILE *warc_tmp = NULL;
  int warcerr = 0;
  int flags = 0;

  if (opt.warc_filename != NULL)
    {
      /* Open a temporary file where we can write the response before we
         add it to the WARC record.  */
      warc_tmp = warc_tempfile ();
      if (warc_tmp == NULL)
        warcerr = WARC_TMP_FOPENERR;

      if (warcerr == 0)
        {
          /* We should keep the response headers for the WARC record.  */
          int head_len = strlen (head);
          int warc_tmp_written = fwrite (head, 1, head_len, warc_tmp);
          if (warc_tmp_written != head_len)
            warcerr = WARC_TMP_FWRITEERR;
          warc_payload_offset = head_len;
        }

      if (warcerr != 0)
        {
          if (warc_tmp != NULL)
            fclose (warc_tmp);
          return warcerr;
        }
    }

  if (fp != NULL)
    {
      /* This confuses the timestamping code that checks for file size.
         #### The timestamping code should be smarter about file size.  */
      if (opt.save_headers && hs->restval == 0)
        fwrite (head, 1, strlen (head), fp);
    }

  /* Read the response body.  */
  if (contlen != -1)
    /* If content-length is present, read that much; otherwise, read
       until EOF.  The HTTP spec doesn't require the server to
       actually close the connection when it's done sending data. */
    flags |= rb_read_exactly;
  if (fp != NULL && hs->restval > 0 && contrange == 0)
    /* If the server ignored our range request, instruct fd_read_body
       to skip the first RESTVAL bytes of body.  */
    flags |= rb_skip_startpos;
  if (chunked_transfer_encoding)
    flags |= rb_chunked_transfer_encoding;

  if (hs->remote_encoding == ENC_GZIP)
    flags |= rb_compressed_gzip;

  hs->len = hs->restval;
  hs->rd_size = 0;
  /* Download the response body and write it to fp.
     If we are working on a WARC file, we simultaneously write the
     response body to warc_tmp.  */
  hs->res = fd_read_body (hs->local_file, sock, fp, contlen != -1 ? contlen : 0,
                          hs->restval, &hs->rd_size, &hs->len, &hs->dltime,
                          flags, warc_tmp);
  if (hs->res >= 0)
    {
      if (warc_tmp != NULL)
        {
          /* Create a response record and write it to the WARC file.
             Note: per the WARC standard, the request and response should share
             the same date header.  We re-use the timestamp of the request.
             The response record should also refer to the uuid of the request.  */
          bool r = warc_write_response_record (url, warc_timestamp_str,
                                               warc_request_uuid, warc_ip,
                                               warc_tmp, warc_payload_offset,
                                               type, statcode, hs->newloc);

          /* warc_write_response_record has closed warc_tmp. */

          if (! r)
            return WARC_ERR;
        }

      return RETRFINISHED;
    }

  if (warc_tmp != NULL)
    fclose (warc_tmp);

  if (hs->res == -2)
    {
      /* Error while writing to fd. */
      return FWRITEERR;
    }
  else if (hs->res == -3)
    {
      /* Error while writing to warc_tmp. */
      return WARC_TMP_FWRITEERR;
    }
  else
    {
      /* A read error! */
      hs->rderrmsg = xstrdup (fd_errstr (sock));
      return RETRFINISHED;
    }
}