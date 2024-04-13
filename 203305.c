check_reply_auth(CMD_Reply *msg, int len)
{
  int data_len;

  data_len = PKL_ReplyLength(msg);

  assert(auth_hash_id >= 0);

  return UTI_CheckNTPAuth(auth_hash_id, (unsigned char *)password, password_length,
      (unsigned char *)msg, data_len,
      ((unsigned char *)msg) + data_len, len - data_len);
}