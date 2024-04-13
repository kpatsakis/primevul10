transform_stat_info (int typeflag, struct tar_stat_info *stat_info)
{
  if (typeflag == GNUTYPE_VOLHDR)
    /* Name transformations don't apply to volume headers. */
    return;

  transform_member_name (&stat_info->file_name, XFORM_REGFILE);
  switch (typeflag)
    {
    case SYMTYPE:
      transform_member_name (&stat_info->link_name, XFORM_SYMLINK);
      break;

    case LNKTYPE:
      transform_member_name (&stat_info->link_name, XFORM_LINK);
    }

  if (one_top_level_option)
    enforce_one_top_level (&current_stat_info.file_name);
}