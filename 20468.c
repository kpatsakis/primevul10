fix_unset_addr_list(UnsetAddrList* uslist, regex_t* reg)
{
  int i, offset;
  EnclosureNode* en;
  AbsAddrType addr;

  for (i = 0; i < uslist->num; i++) {
    if (! NODE_IS_ADDR_FIXED(uslist->us[i].target))
      return ONIGERR_PARSER_BUG;

    en = ENCLOSURE_(uslist->us[i].target);
    addr   = en->m.called_addr;
    offset = uslist->us[i].offset;

    BB_WRITE(reg, offset, &addr, SIZE_ABSADDR);
  }
  return 0;
}