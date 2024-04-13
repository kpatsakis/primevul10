post_process_multipart (struct MHD_PostProcessor *pp,
                        const char *post_data,
                        size_t post_data_len)
{
  char *buf;
  size_t max;
  size_t ioff;
  size_t poff;
  int state_changed;

  buf = (char *) &pp[1];
  ioff = 0;
  poff = 0;
  state_changed = 1;
  while ( (poff < post_data_len) ||
          ( (pp->buffer_pos > 0) &&
            (0 != state_changed) ) )
  {
    /* first, move as much input data
       as possible to our internal buffer */
    max = pp->buffer_size - pp->buffer_pos;
    if (max > post_data_len - poff)
      max = post_data_len - poff;
    memcpy (&buf[pp->buffer_pos],
            &post_data[poff],
            max);
    poff += max;
    pp->buffer_pos += max;
    if ( (0 == max) &&
         (0 == state_changed) &&
         (poff < post_data_len) )
    {
      pp->state = PP_Error;
      return MHD_NO;            /* out of memory */
    }
    state_changed = 0;

    /* first state machine for '\r'-'\n' and '--' handling */
    switch (pp->skip_rn)
    {
    case RN_Inactive:
      break;
    case RN_OptN:
      if (buf[0] == '\n')
      {
        ioff++;
        pp->skip_rn = RN_Inactive;
        goto AGAIN;
      }
    /* fall-through! */
    case RN_Dash:
      if (buf[0] == '-')
      {
        ioff++;
        pp->skip_rn = RN_Dash2;
        goto AGAIN;
      }
      pp->skip_rn = RN_Full;
    /* fall-through! */
    case RN_Full:
      if (buf[0] == '\r')
      {
        if ( (pp->buffer_pos > 1) &&
             ('\n' == buf[1]) )
        {
          pp->skip_rn = RN_Inactive;
          ioff += 2;
        }
        else
        {
          pp->skip_rn = RN_OptN;
          ioff++;
        }
        goto AGAIN;
      }
      if (buf[0] == '\n')
      {
        ioff++;
        pp->skip_rn = RN_Inactive;
        goto AGAIN;
      }
      pp->skip_rn = RN_Inactive;
      pp->state = PP_Error;
      return MHD_NO;            /* no '\r\n' */
    case RN_Dash2:
      if (buf[0] == '-')
      {
        ioff++;
        pp->skip_rn = RN_Full;
        pp->state = pp->dash_state;
        goto AGAIN;
      }
      pp->state = PP_Error;
      break;
    }

    /* main state engine */
    switch (pp->state)
    {
    case PP_Error:
      return MHD_NO;
    case PP_Done:
      /* did not expect to receive more data */
      pp->state = PP_Error;
      return MHD_NO;
    case PP_Init:
      /**
       * Per RFC2046 5.1.1 NOTE TO IMPLEMENTORS, consume anything
       * prior to the first multipart boundary:
       *
       * > There appears to be room for additional information prior
       * > to the first boundary delimiter line and following the
       * > final boundary delimiter line.  These areas should
       * > generally be left blank, and implementations must ignore
       * > anything that appears before the first boundary delimiter
       * > line or after the last one.
       */(void) find_boundary (pp,
                            pp->boundary,
                            pp->blen,
                            &ioff,
                            PP_ProcessEntryHeaders,
                            PP_Done);
      break;
    case PP_NextBoundary:
      if (MHD_NO == find_boundary (pp,
                                   pp->boundary,
                                   pp->blen,
                                   &ioff,
                                   PP_ProcessEntryHeaders,
                                   PP_Done))
      {
        if (pp->state == PP_Error)
          return MHD_NO;
        goto END;
      }
      break;
    case PP_ProcessEntryHeaders:
      pp->must_ikvi = true;
      if (MHD_NO ==
          process_multipart_headers (pp,
                                     &ioff,
                                     PP_PerformCheckMultipart))
      {
        if (pp->state == PP_Error)
          return MHD_NO;
        else
          goto END;
      }
      state_changed = 1;
      break;
    case PP_PerformCheckMultipart:
      if ( (NULL != pp->content_type) &&
           (MHD_str_equal_caseless_n_ (pp->content_type,
                                       "multipart/mixed",
                                       MHD_STATICSTR_LEN_ ("multipart/mixed"))))
      {
        pp->nested_boundary = strstr (pp->content_type,
                                      "boundary=");
        if (NULL == pp->nested_boundary)
        {
          pp->state = PP_Error;
          return MHD_NO;
        }
        pp->nested_boundary =
          strdup (&pp->nested_boundary[MHD_STATICSTR_LEN_ ("boundary=")]);
        if (NULL == pp->nested_boundary)
        {
          /* out of memory */
          pp->state = PP_Error;
          return MHD_NO;
        }
        /* free old content type, we will need that field
           for the content type of the nested elements */
        free (pp->content_type);
        pp->content_type = NULL;
        pp->nlen = strlen (pp->nested_boundary);
        pp->state = PP_Nested_Init;
        state_changed = 1;
        break;
      }
      pp->state = PP_ProcessValueToBoundary;
      pp->value_offset = 0;
      state_changed = 1;
      break;
    case PP_ProcessValueToBoundary:
      if (MHD_NO == process_value_to_boundary (pp,
                                               &ioff,
                                               pp->boundary,
                                               pp->blen,
                                               PP_PerformCleanup,
                                               PP_Done))
      {
        if (pp->state == PP_Error)
          return MHD_NO;
        break;
      }
      break;
    case PP_PerformCleanup:
      /* clean up state of one multipart form-data element! */
      pp->have = NE_none;
      free_unmarked (pp);
      if (NULL != pp->nested_boundary)
      {
        free (pp->nested_boundary);
        pp->nested_boundary = NULL;
      }
      pp->state = PP_ProcessEntryHeaders;
      state_changed = 1;
      break;
    case PP_Nested_Init:
      if (NULL == pp->nested_boundary)
      {
        pp->state = PP_Error;
        return MHD_NO;
      }
      if (MHD_NO == find_boundary (pp,
                                   pp->nested_boundary,
                                   pp->nlen,
                                   &ioff,
                                   PP_Nested_PerformMarking,
                                   PP_NextBoundary /* or PP_Error? */))
      {
        if (pp->state == PP_Error)
          return MHD_NO;
        goto END;
      }
      break;
    case PP_Nested_PerformMarking:
      /* remember what headers were given
         globally */
      pp->have = NE_none;
      if (NULL != pp->content_name)
        pp->have |= NE_content_name;
      if (NULL != pp->content_type)
        pp->have |= NE_content_type;
      if (NULL != pp->content_filename)
        pp->have |= NE_content_filename;
      if (NULL != pp->content_transfer_encoding)
        pp->have |= NE_content_transfer_encoding;
      pp->state = PP_Nested_ProcessEntryHeaders;
      state_changed = 1;
      break;
    case PP_Nested_ProcessEntryHeaders:
      pp->value_offset = 0;
      if (MHD_NO ==
          process_multipart_headers (pp,
                                     &ioff,
                                     PP_Nested_ProcessValueToBoundary))
      {
        if (pp->state == PP_Error)
          return MHD_NO;
        else
          goto END;
      }
      state_changed = 1;
      break;
    case PP_Nested_ProcessValueToBoundary:
      if (MHD_NO == process_value_to_boundary (pp,
                                               &ioff,
                                               pp->nested_boundary,
                                               pp->nlen,
                                               PP_Nested_PerformCleanup,
                                               PP_NextBoundary))
      {
        if (pp->state == PP_Error)
          return MHD_NO;
        break;
      }
      break;
    case PP_Nested_PerformCleanup:
      free_unmarked (pp);
      pp->state = PP_Nested_ProcessEntryHeaders;
      state_changed = 1;
      break;
    default:
      mhd_panic (mhd_panic_cls,
                 __FILE__,
                 __LINE__,
                 NULL);              /* should never happen! */
    }
AGAIN:
    if (ioff > 0)
    {
      memmove (buf,
               &buf[ioff],
               pp->buffer_pos - ioff);
      pp->buffer_pos -= ioff;
      ioff = 0;
      state_changed = 1;
    }
  }
END:
  if (0 != ioff)
  {
    memmove (buf,
             &buf[ioff],
             pp->buffer_pos - ioff);
    pp->buffer_pos -= ioff;
  }
  if (poff < post_data_len)
  {
    pp->state = PP_Error;
    return MHD_NO;              /* serious error */
  }
  return MHD_YES;
}