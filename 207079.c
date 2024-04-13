attlist_helper (void *item, void *data)
{
  struct atchinfo *aptr = item;
  mu_list_t list = data;
  attlist_add (list, aptr->id, aptr->encoding, aptr->content_type,
	       aptr->name, aptr->filename, aptr->source, aptr->skip_empty,
	       aptr->disp_inline);
  return 0;
}