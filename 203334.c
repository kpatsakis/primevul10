process_cmd_activity(const char *line)
{
  CMD_Request request;
  CMD_Reply reply;
  request.command = htons(REQ_ACTIVITY);
  if (request_reply(&request, &reply, RPY_ACTIVITY, 1)) {
        printf(
               "%ld sources online\n"
               "%ld sources offline\n"
               "%ld sources doing burst (return to online)\n"
               "%ld sources doing burst (return to offline)\n"
               "%ld sources with unknown address\n",
                (long) ntohl(reply.data.activity.online),
                (long) ntohl(reply.data.activity.offline),
                (long) ntohl(reply.data.activity.burst_online),
                (long) ntohl(reply.data.activity.burst_offline),
                (long) ntohl(reply.data.activity.unresolved));
        return 1;
  }
  return 0;
}