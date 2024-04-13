take_offline(NCR_Instance inst)
{
  inst->opmode = MD_OFFLINE;
  if (inst->timer_running) {
    SCH_RemoveTimeout(inst->timeout_id);
    inst->timer_running = 0;
  }

  /* Mark source unreachable */
  SRC_ResetReachability(inst->source);

  /* And inactive */
  SRC_UnsetActive(inst->source);

  close_client_socket(inst);

  NCR_ResetInstance(inst);
}