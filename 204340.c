transform_member_name (char **pinput, int type)
{
  return transform_name_fp (pinput, type, decode_xform, &type);
}