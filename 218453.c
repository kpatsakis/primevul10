duplicate_node (re_dfa_t *dfa, Idx org_idx, unsigned int constraint)
{
  Idx dup_idx = re_dfa_add_node (dfa, dfa->nodes[org_idx]);
  if (BE (dup_idx != REG_MISSING, 1))
    {
      dfa->nodes[dup_idx].constraint = constraint;
      dfa->nodes[dup_idx].constraint |= dfa->nodes[org_idx].constraint;
      dfa->nodes[dup_idx].duplicated = 1;

      /* Store the index of the original node.  */
      dfa->org_indices[dup_idx] = org_idx;
    }
  return dup_idx;
}