process_cmd_burst(CMD_Request *msg, char *line)
{
  int n_good_samples, n_total_samples;
  char *s1, *s2;
  IPAddr address, mask;

  s1 = line;
  s2 = CPS_SplitWord(s1);
  CPS_SplitWord(s2);

  if (sscanf(s1, "%d/%d", &n_good_samples, &n_total_samples) != 2) {
    fprintf(stderr, "Invalid syntax for burst command\n");
    return 0;
  }

  mask.family = address.family = IPADDR_UNSPEC;
  if (*s2 && !read_mask_address(s2, &mask, &address)) {
    return 0;
  }

  msg->command = htons(REQ_BURST);
  msg->data.burst.n_good_samples = ntohl(n_good_samples);
  msg->data.burst.n_total_samples = ntohl(n_total_samples);

  UTI_IPHostToNetwork(&mask, &msg->data.burst.mask);
  UTI_IPHostToNetwork(&address, &msg->data.burst.address);

  return 1;
}