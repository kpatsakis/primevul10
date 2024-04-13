posix_state_to_acl(struct posix_acl_state *state, unsigned int flags)
{
	struct posix_acl_entry *pace;
	struct posix_acl *pacl;
	int nace;
	int i, error = 0;

	/*
	 * ACLs with no ACEs are treated differently in the inheritable
	 * and effective cases: when there are no inheritable ACEs, we
	 * set a zero-length default posix acl:
	 */
	if (state->empty && (flags & NFS4_ACL_TYPE_DEFAULT)) {
		pacl = posix_acl_alloc(0, GFP_KERNEL);
		return pacl ? pacl : ERR_PTR(-ENOMEM);
	}
	/*
	 * When there are no effective ACEs, the following will end
	 * up setting a 3-element effective posix ACL with all
	 * permissions zero.
	 */
	nace = 4 + state->users->n + state->groups->n;
	pacl = posix_acl_alloc(nace, GFP_KERNEL);
	if (!pacl)
		return ERR_PTR(-ENOMEM);

	pace = pacl->a_entries;
	pace->e_tag = ACL_USER_OBJ;
	error = check_deny(state->owner.deny, 1);
	if (error)
		goto out_err;
	low_mode_from_nfs4(state->owner.allow, &pace->e_perm, flags);
	pace->e_id = ACL_UNDEFINED_ID;

	for (i=0; i < state->users->n; i++) {
		pace++;
		pace->e_tag = ACL_USER;
		error = check_deny(state->users->aces[i].perms.deny, 0);
		if (error)
			goto out_err;
		low_mode_from_nfs4(state->users->aces[i].perms.allow,
					&pace->e_perm, flags);
		pace->e_id = state->users->aces[i].uid;
		add_to_mask(state, &state->users->aces[i].perms);
	}

	pace++;
	pace->e_tag = ACL_GROUP_OBJ;
	error = check_deny(state->group.deny, 0);
	if (error)
		goto out_err;
	low_mode_from_nfs4(state->group.allow, &pace->e_perm, flags);
	pace->e_id = ACL_UNDEFINED_ID;
	add_to_mask(state, &state->group);

	for (i=0; i < state->groups->n; i++) {
		pace++;
		pace->e_tag = ACL_GROUP;
		error = check_deny(state->groups->aces[i].perms.deny, 0);
		if (error)
			goto out_err;
		low_mode_from_nfs4(state->groups->aces[i].perms.allow,
					&pace->e_perm, flags);
		pace->e_id = state->groups->aces[i].uid;
		add_to_mask(state, &state->groups->aces[i].perms);
	}

	pace++;
	pace->e_tag = ACL_MASK;
	low_mode_from_nfs4(state->mask.allow, &pace->e_perm, flags);
	pace->e_id = ACL_UNDEFINED_ID;

	pace++;
	pace->e_tag = ACL_OTHER;
	error = check_deny(state->other.deny, 0);
	if (error)
		goto out_err;
	low_mode_from_nfs4(state->other.allow, &pace->e_perm, flags);
	pace->e_id = ACL_UNDEFINED_ID;

	return pacl;
out_err:
	posix_acl_release(pacl);
	return ERR_PTR(error);
}