ins_compl_add_matches(
    int		num_matches,
    char_u	**matches,
    int		icase)
{
    int		i;
    int		add_r = OK;
    int		dir = compl_direction;

    for (i = 0; i < num_matches && add_r != FAIL; i++)
	if ((add_r = ins_compl_add(matches[i], -1, NULL, NULL, NULL, dir,
			       CP_FAST | (icase ? CP_ICASE : 0), FALSE)) == OK)
	    // if dir was BACKWARD then honor it just once
	    dir = FORWARD;
    FreeWild(num_matches, matches);
}