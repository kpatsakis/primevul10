maybe_reverse (char const *name, bool nonexistent, bool is_empty)
{
  bool looks_reversed = (! is_empty) < p_says_nonexistent[reverse ^ is_empty];

  /* Allow to create and delete empty files when we know that they are empty:
     in the "diff --git" format, we know that from the index header.  */
  if (is_empty
      && p_says_nonexistent[reverse ^ nonexistent] == 1
      && p_says_nonexistent[! reverse ^ nonexistent] == 2)
    return false;

  if (looks_reversed)
    reverse ^=
      ok_to_reverse ("The next patch%s would %s the file %s,\nwhich %s!",
		     reverse ? ", when reversed," : "",
		     (nonexistent ? "delete"
		      : is_empty ? "empty out"
		      : "create"),
		     quotearg (name),
		     (nonexistent ? "does not exist"
		      : is_empty ? "is already empty"
		      : "already exists"));
  return looks_reversed;
}