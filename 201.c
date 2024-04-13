QuotaManagerProxy::QuotaManagerProxy(
    QuotaManager* manager, base::MessageLoopProxy* io_thread)
    : manager_(manager), io_thread_(io_thread) {
}
