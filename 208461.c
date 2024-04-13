static char_u *get_varp_scope(vimoption_T *p, int opt_flags)
{
  if ((opt_flags & OPT_GLOBAL) && p->indir != PV_NONE) {
    if (p->var == VAR_WIN)
      return (char_u *)GLOBAL_WO(get_varp(p));
    return p->var;
  }
  if ((opt_flags & OPT_LOCAL) && ((int)p->indir & PV_BOTH)) {
    switch ((int)p->indir) {
    case PV_EFM:  return (char_u *)&(curbuf->b_p_efm);
    case PV_GP:   return (char_u *)&(curbuf->b_p_gp);
    case PV_MP:   return (char_u *)&(curbuf->b_p_mp);
    case PV_EP:   return (char_u *)&(curbuf->b_p_ep);
    case PV_KP:   return (char_u *)&(curbuf->b_p_kp);
    case PV_PATH: return (char_u *)&(curbuf->b_p_path);
    case PV_AR:   return (char_u *)&(curbuf->b_p_ar);
    case PV_TAGS: return (char_u *)&(curbuf->b_p_tags);
    case PV_TC:   return (char_u *)&(curbuf->b_p_tc);
    case PV_DEF:  return (char_u *)&(curbuf->b_p_def);
    case PV_INC:  return (char_u *)&(curbuf->b_p_inc);
    case PV_DICT: return (char_u *)&(curbuf->b_p_dict);
    case PV_TSR:  return (char_u *)&(curbuf->b_p_tsr);
    case PV_STL:  return (char_u *)&(curwin->w_p_stl);
    case PV_UL:   return (char_u *)&(curbuf->b_p_ul);
    case PV_LW:   return (char_u *)&(curbuf->b_p_lw);
    case PV_BKC:  return (char_u *)&(curbuf->b_p_bkc);
    }
    return NULL;     /* "cannot happen" */
  }
  return get_varp(p);
}