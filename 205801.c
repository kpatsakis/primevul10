bool Item_field::subst_argument_checker(uchar **arg)
{
  return *arg &&
         (*arg == (uchar *) Item::ANY_SUBST ||
          result_type() != STRING_RESULT || 
          (field->flags & BINARY_FLAG));
}