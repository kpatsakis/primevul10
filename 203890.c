int NCR_IsSyncPeer(NCR_Instance inst)
{
  return SRC_IsSyncPeer(inst->source);
}