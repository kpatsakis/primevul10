std::vector<printing::PrinterBasicInfo> WebContents::GetPrinterList() {
  std::vector<printing::PrinterBasicInfo> printers;
  auto print_backend = printing::PrintBackend::CreateInstance(
      nullptr, g_browser_process->GetApplicationLocale());
  {
    // TODO(deepak1556): Deprecate this api in favor of an
    // async version and post a non blocing task call.
    base::ThreadRestrictions::ScopedAllowIO allow_io;
    print_backend->EnumeratePrinters(&printers);
  }
  return printers;
}