prev_insert_id(ulonglong nr, struct system_variables *variables)
{
  if (unlikely(nr < variables->auto_increment_offset))
  {
    /*
      There's nothing good we can do here. That is a pathological case, where
      the offset is larger than the column's max possible value, i.e. not even
      the first sequence value may be inserted. User will receive warning.
    */
    DBUG_PRINT("info",("auto_increment: nr: %lu cannot honour "
                       "auto_increment_offset: %lu",
                       (ulong) nr, variables->auto_increment_offset));
    return nr;
  }
  if (variables->auto_increment_increment == 1)
    return nr; // optimization of the formula below
  /*
     Calculating the number of complete auto_increment_increment extents:
  */
  nr= (nr - variables->auto_increment_offset) /
      (ulonglong) variables->auto_increment_increment;
  /*
     Adding an offset to the auto_increment_increment extent boundary:
  */
  return (nr * (ulonglong) variables->auto_increment_increment +
          variables->auto_increment_offset);
}