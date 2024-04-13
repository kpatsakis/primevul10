Item *Item_int_with_ref::clone_item()
{
  DBUG_ASSERT(ref->const_item());
  /*
    We need to evaluate the constant to make sure it works with
    parameter markers.
  */
  return (ref->unsigned_flag ?
          new Item_uint(ref->name, ref->val_int(), ref->max_length) :
          new Item_int(ref->name, ref->val_int(), ref->max_length));
}