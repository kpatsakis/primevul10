trigger_complete_changed_event(int cur)
{
    dict_T	    *v_event;
    dict_T	    *item;
    static int	    recursive = FALSE;
    save_v_event_T  save_v_event;

    if (recursive)
	return;

    if (cur < 0)
	item = dict_alloc();
    else
	item = ins_compl_dict_alloc(compl_curr_match);
    if (item == NULL)
	return;
    v_event = get_v_event(&save_v_event);
    dict_add_dict(v_event, "completed_item", item);
    pum_set_event_info(v_event);
    dict_set_items_ro(v_event);

    recursive = TRUE;
    textlock++;
    apply_autocmds(EVENT_COMPLETECHANGED, NULL, NULL, FALSE, curbuf);
    textlock--;
    recursive = FALSE;

    restore_v_event(v_event, &save_v_event);
}