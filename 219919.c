u_check(int newhead_may_be_NULL)
{
    seen_b_u_newhead = 0;
    seen_b_u_curhead = 0;
    header_count = 0;

    u_check_tree(curbuf->b_u_oldhead, NULL, NULL);

    if (seen_b_u_newhead == 0 && curbuf->b_u_oldhead != NULL
	    && !(newhead_may_be_NULL && curbuf->b_u_newhead == NULL))
	EMSGN("b_u_newhead invalid: 0x%x", curbuf->b_u_newhead);
    if (curbuf->b_u_curhead != NULL && seen_b_u_curhead == 0)
	EMSGN("b_u_curhead invalid: 0x%x", curbuf->b_u_curhead);
    if (header_count != curbuf->b_u_numhead)
    {
	EMSG("b_u_numhead invalid");
	smsg((char_u *)"expected: %ld, actual: %ld",
			       (long)header_count, (long)curbuf->b_u_numhead);
    }
}