generate_auth(CMD_Request *msg)
{
  int data_len;

  data_len = PKL_CommandLength(msg);

  assert(auth_hash_id >= 0);

  return UTI_GenerateNTPAuth(auth_hash_id, (unsigned char *)password, password_length,
      (unsigned char *)msg, data_len, ((unsigned char *)msg) + data_len, sizeof (msg->auth));
}