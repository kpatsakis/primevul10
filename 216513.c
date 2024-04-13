void WebContents::InspectSharedWorkerById(const std::string& workerId) {
  if (type_ == Type::REMOTE)
    return;

  if (!enable_devtools_)
    return;

  for (const auto& agent_host : content::DevToolsAgentHost::GetOrCreateAll()) {
    if (agent_host->GetType() ==
        content::DevToolsAgentHost::kTypeSharedWorker) {
      if (agent_host->GetId() == workerId) {
        OpenDevTools(nullptr);
        managed_web_contents()->AttachTo(agent_host);
        break;
      }
    }
  }
}