best_name (char *const *name, int const *ignore)
{
  enum nametype i;
  int components[3];
  int components_min = INT_MAX;
  size_t basename_len[3];
  size_t basename_len_min = SIZE_MAX;
  size_t len[3];
  size_t len_min = SIZE_MAX;

  for (i = OLD;  i <= INDEX;  i++)
    if (name[i] && !ignore[i])
      {
	/* Take the names with the fewest prefix components.  */
	components[i] = prefix_components (name[i], false);
	if (components_min < components[i])
	  continue;
	components_min = components[i];

	/* Of those, take the names with the shortest basename.  */
	basename_len[i] = base_len (name[i]);
	if (basename_len_min < basename_len[i])
	  continue;
	basename_len_min = basename_len[i];

	/* Of those, take the shortest names.  */
	len[i] = strlen (name[i]);
	if (len_min < len[i])
	  continue;
	len_min = len[i];
      }

  /* Of those, take the first name.  */
  for (i = OLD;  i <= INDEX;  i++)
    if (name[i] && !ignore[i]
	&& name_is_valid (name[i])
	&& components[i] == components_min
	&& basename_len[i] == basename_len_min
	&& len[i] == len_min)
      break;

  return i;
}