void Item_trigger_field::set_required_privilege(bool rw)
{
  /*
    Require SELECT and UPDATE privilege if this field will be read and
    set, and only UPDATE privilege for setting the field.
  */
  want_privilege= (rw ? SELECT_ACL | UPDATE_ACL : UPDATE_ACL);
}