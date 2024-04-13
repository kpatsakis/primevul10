void unset_global_local_option(char *name, void *from)
{
  vimoption_T *p;
  buf_T *buf = (buf_T *)from;

  int opt_idx = findoption((uint8_t *)name);
  if (opt_idx < 0) {
    EMSG2(_("E355: Unknown option: %s"), name);
    return;
  }
  p = &(options[opt_idx]);

  switch ((int)p->indir)
  {
    // global option with local value: use local value if it's been set
    case PV_EP:
      clear_string_option(&buf->b_p_ep);
      break;
    case PV_KP:
      clear_string_option(&buf->b_p_kp);
      break;
    case PV_PATH:
      clear_string_option(&buf->b_p_path);
      break;
    case PV_AR:
      buf->b_p_ar = -1;
      break;
    case PV_BKC:
      clear_string_option(&buf->b_p_bkc);
      buf->b_bkc_flags = 0;
      break;
    case PV_TAGS:
      clear_string_option(&buf->b_p_tags);
      break;
    case PV_TC:
      clear_string_option(&buf->b_p_tc);
      buf->b_tc_flags = 0;
      break;
    case PV_DEF:
      clear_string_option(&buf->b_p_def);
      break;
    case PV_INC:
      clear_string_option(&buf->b_p_inc);
      break;
    case PV_DICT:
      clear_string_option(&buf->b_p_dict);
      break;
    case PV_TSR:
      clear_string_option(&buf->b_p_tsr);
      break;
    case PV_EFM:
      clear_string_option(&buf->b_p_efm);
      break;
    case PV_GP:
      clear_string_option(&buf->b_p_gp);
      break;
    case PV_MP:
      clear_string_option(&buf->b_p_mp);
      break;
    case PV_STL:
      clear_string_option(&((win_T *)from)->w_p_stl);
      break;
    case PV_UL:
      buf->b_p_ul = NO_LOCAL_UNDOLEVEL;
      break;
    case PV_LW:
      clear_string_option(&buf->b_p_lw);
      break;
  }
}