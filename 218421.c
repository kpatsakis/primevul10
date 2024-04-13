duplicate_node_closure (re_dfa_t *dfa, Idx top_org_node, Idx top_clone_node,
			Idx root_node, unsigned int init_constraint)
{
  Idx org_node, clone_node;
  bool ok;
  unsigned int constraint = init_constraint;
  for (org_node = top_org_node, clone_node = top_clone_node;;)
    {
      Idx org_dest, clone_dest;
      if (dfa->nodes[org_node].type == OP_BACK_REF)
	{
	  /* If the back reference epsilon-transit, its destination must
	     also have the constraint.  Then duplicate the epsilon closure
	     of the destination of the back reference, and store it in
	     edests of the back reference.  */
	  org_dest = dfa->nexts[org_node];
	  re_node_set_empty (dfa->edests + clone_node);
	  clone_dest = duplicate_node (dfa, org_dest, constraint);
	  if (BE (clone_dest == REG_MISSING, 0))
	    return REG_ESPACE;
	  dfa->nexts[clone_node] = dfa->nexts[org_node];
	  ok = re_node_set_insert (dfa->edests + clone_node, clone_dest);
	  if (BE (! ok, 0))
	    return REG_ESPACE;
	}
      else if (dfa->edests[org_node].nelem == 0)
	{
	  /* In case of the node can't epsilon-transit, don't duplicate the
	     destination and store the original destination as the
	     destination of the node.  */
	  dfa->nexts[clone_node] = dfa->nexts[org_node];
	  break;
	}
      else if (dfa->edests[org_node].nelem == 1)
	{
	  /* In case of the node can epsilon-transit, and it has only one
	     destination.  */
	  org_dest = dfa->edests[org_node].elems[0];
	  re_node_set_empty (dfa->edests + clone_node);
	  /* If the node is root_node itself, it means the epsilon closure
	     has a loop.  Then tie it to the destination of the root_node.  */
	  if (org_node == root_node && clone_node != org_node)
	    {
	      ok = re_node_set_insert (dfa->edests + clone_node, org_dest);
	      if (BE (! ok, 0))
	        return REG_ESPACE;
	      break;
	    }
	  /* In case the node has another constraint, append it.  */
	  constraint |= dfa->nodes[org_node].constraint;
	  clone_dest = duplicate_node (dfa, org_dest, constraint);
	  if (BE (clone_dest == REG_MISSING, 0))
	    return REG_ESPACE;
	  ok = re_node_set_insert (dfa->edests + clone_node, clone_dest);
	  if (BE (! ok, 0))
	    return REG_ESPACE;
	}
      else /* dfa->edests[org_node].nelem == 2 */
	{
	  /* In case of the node can epsilon-transit, and it has two
	     destinations. In the bin_tree_t and DFA, that's '|' and '*'.   */
	  org_dest = dfa->edests[org_node].elems[0];
	  re_node_set_empty (dfa->edests + clone_node);
	  /* Search for a duplicated node which satisfies the constraint.  */
	  clone_dest = search_duplicated_node (dfa, org_dest, constraint);
	  if (clone_dest == REG_MISSING)
	    {
	      /* There is no such duplicated node, create a new one.  */
	      reg_errcode_t err;
	      clone_dest = duplicate_node (dfa, org_dest, constraint);
	      if (BE (clone_dest == REG_MISSING, 0))
		return REG_ESPACE;
	      ok = re_node_set_insert (dfa->edests + clone_node, clone_dest);
	      if (BE (! ok, 0))
		return REG_ESPACE;
	      err = duplicate_node_closure (dfa, org_dest, clone_dest,
					    root_node, constraint);
	      if (BE (err != REG_NOERROR, 0))
		return err;
	    }
	  else
	    {
	      /* There is a duplicated node which satisfies the constraint,
		 use it to avoid infinite loop.  */
	      ok = re_node_set_insert (dfa->edests + clone_node, clone_dest);
	      if (BE (! ok, 0))
		return REG_ESPACE;
	    }

	  org_dest = dfa->edests[org_node].elems[1];
	  clone_dest = duplicate_node (dfa, org_dest, constraint);
	  if (BE (clone_dest == REG_MISSING, 0))
	    return REG_ESPACE;
	  ok = re_node_set_insert (dfa->edests + clone_node, clone_dest);
	  if (BE (! ok, 0))
	    return REG_ESPACE;
	}
      org_node = org_dest;
      clone_node = clone_dest;
    }
  return REG_NOERROR;
}