theme_adium_find_abilities (EmpathyChatView *view,
			    const gchar    *search_criteria,
                            gboolean        match_case,
			    gboolean       *can_do_previous,
			    gboolean       *can_do_next)
{
	/* FIXME: Does webkit provide an API for that? We have wrap=true in
	 * find_next and find_previous to work around this problem. */
	if (can_do_previous)
		*can_do_previous = TRUE;
	if (can_do_next)
		*can_do_next = TRUE;
}