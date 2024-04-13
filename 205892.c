bool Item_outer_ref::check_inner_refs_processor(uchar *arg)
{
  List_iterator_fast<Item_outer_ref> *it=
    ((List_iterator_fast<Item_outer_ref> *) arg);
  Item_outer_ref *ref;
  while ((ref= (*it)++))
  {
    if (ref == this)
    {
      ref->found_in_group_by= 1;
      break;
    }
  }
  (*it).rewind();
  return FALSE;
}