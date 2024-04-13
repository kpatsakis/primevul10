free_unmarked (struct MHD_PostProcessor *pp)
{
  if ( (NULL != pp->content_name) &&
       (0 == (pp->have & NE_content_name)) )
  {
    free (pp->content_name);
    pp->content_name = NULL;
  }
  if ( (NULL != pp->content_type) &&
       (0 == (pp->have & NE_content_type)) )
  {
    free (pp->content_type);
    pp->content_type = NULL;
  }
  if ( (NULL != pp->content_filename) &&
       (0 == (pp->have & NE_content_filename)) )
  {
    free (pp->content_filename);
    pp->content_filename = NULL;
  }
  if ( (NULL != pp->content_transfer_encoding) &&
       (0 == (pp->have & NE_content_transfer_encoding)) )
  {
    free (pp->content_transfer_encoding);
    pp->content_transfer_encoding = NULL;
  }
}