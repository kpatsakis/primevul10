handle_activity(CMD_Request *rx_message, CMD_Reply *tx_message)
{
  RPT_ActivityReport report;
  NSR_GetActivityReport(&report);
  tx_message->data.activity.online = htonl(report.online);
  tx_message->data.activity.offline = htonl(report.offline);
  tx_message->data.activity.burst_online = htonl(report.burst_online);
  tx_message->data.activity.burst_offline = htonl(report.burst_offline);
  tx_message->data.activity.unresolved = htonl(report.unresolved);
  tx_message->status = htons(STT_SUCCESS);
  tx_message->reply = htons(RPY_ACTIVITY);
}