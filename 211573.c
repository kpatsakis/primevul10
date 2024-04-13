Variant HHVM_FUNCTION(nl_langinfo, int item) {
#ifdef _MSC_VER
  raise_warning("nl_langinfo is not yet implemented on Windows!");
  return empty_string();
#else
  switch(item) {
#ifdef ABDAY_1
    case ABDAY_1:
    case ABDAY_2:
    case ABDAY_3:
    case ABDAY_4:
    case ABDAY_5:
    case ABDAY_6:
    case ABDAY_7:
#endif
#ifdef DAY_1
    case DAY_1:
    case DAY_2:
    case DAY_3:
    case DAY_4:
    case DAY_5:
    case DAY_6:
    case DAY_7:
#endif
#ifdef ABMON_1
    case ABMON_1:
    case ABMON_2:
    case ABMON_3:
    case ABMON_4:
    case ABMON_5:
    case ABMON_6:
    case ABMON_7:
    case ABMON_8:
    case ABMON_9:
    case ABMON_10:
    case ABMON_11:
    case ABMON_12:
#endif
#ifdef MON_1
    case MON_1:
    case MON_2:
    case MON_3:
    case MON_4:
    case MON_5:
    case MON_6:
    case MON_7:
    case MON_8:
    case MON_9:
    case MON_10:
    case MON_11:
    case MON_12:
#endif
#ifdef AM_STR
    case AM_STR:
#endif
#ifdef PM_STR
    case PM_STR:
#endif
#ifdef D_T_FMT
    case D_T_FMT:
#endif
#ifdef D_FMT
    case D_FMT:
#endif
#ifdef T_FMT
    case T_FMT:
#endif
#ifdef T_FMT_AMPM
    case T_FMT_AMPM:
#endif
#ifdef ERA
    case ERA:
#endif
#ifdef ERA_YEAR
    case ERA_YEAR:
#endif
#ifdef ERA_D_T_FMT
    case ERA_D_T_FMT:
#endif
#ifdef ERA_D_FMT
    case ERA_D_FMT:
#endif
#ifdef ERA_T_FMT
    case ERA_T_FMT:
#endif
#ifdef ALT_DIGITS
    case ALT_DIGITS:
#endif
#ifdef INT_CURR_SYMBOL
    case INT_CURR_SYMBOL:
#endif
#ifdef CURRENCY_SYMBOL
    case CURRENCY_SYMBOL:
#endif
#ifdef CRNCYSTR
    case CRNCYSTR:
#endif
#ifdef MON_DECIMAL_POINT
    case MON_DECIMAL_POINT:
#endif
#ifdef MON_THOUSANDS_SEP
    case MON_THOUSANDS_SEP:
#endif
#ifdef MON_GROUPING
    case MON_GROUPING:
#endif
#ifdef POSITIVE_SIGN
    case POSITIVE_SIGN:
#endif
#ifdef NEGATIVE_SIGN
    case NEGATIVE_SIGN:
#endif
#ifdef INT_FRAC_DIGITS
    case INT_FRAC_DIGITS:
#endif
#ifdef FRAC_DIGITS
    case FRAC_DIGITS:
#endif
#ifdef P_CS_PRECEDES
    case P_CS_PRECEDES:
#endif
#ifdef P_SEP_BY_SPACE
    case P_SEP_BY_SPACE:
#endif
#ifdef N_CS_PRECEDES
    case N_CS_PRECEDES:
#endif
#ifdef N_SEP_BY_SPACE
    case N_SEP_BY_SPACE:
#endif
#ifdef P_SIGN_POSN
    case P_SIGN_POSN:
#endif
#ifdef N_SIGN_POSN
    case N_SIGN_POSN:
#endif
#ifdef DECIMAL_POINT
    case DECIMAL_POINT:
#elif defined(RADIXCHAR)
    case RADIXCHAR:
#endif
#ifdef THOUSANDS_SEP
    case THOUSANDS_SEP:
#elif defined(THOUSEP)
    case THOUSEP:
#endif
#ifdef GROUPING
    case GROUPING:
#endif
#ifdef YESEXPR
    case YESEXPR:
#endif
#ifdef NOEXPR
    case NOEXPR:
#endif
#ifdef YESSTR
    case YESSTR:
#endif
#ifdef NOSTR
    case NOSTR:
#endif
#ifdef CODESET
    case CODESET:
#endif
      break;
    default:
      raise_warning("Item '%d' is not valid", item);
      return false;
  }

  auto const ret = nl_langinfo(item);
  if (ret == nullptr) {
    return false;
  }
  return String(ret);
#endif
}