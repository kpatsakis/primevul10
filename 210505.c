bool ContentSettingsStore::OnCorrectThread() {
  return !BrowserThread::IsWellKnownThread(BrowserThread::UI) ||
         BrowserThread::CurrentlyOn(BrowserThread::UI);
}
