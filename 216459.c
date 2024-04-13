base::string16 GetDefaultPrinterAsync() {
  base::ScopedBlockingCall scoped_blocking_call(FROM_HERE,
                                                base::BlockingType::MAY_BLOCK);

  scoped_refptr<printing::PrintBackend> backend =
      printing::PrintBackend::CreateInstance(
          nullptr, g_browser_process->GetApplicationLocale());
  std::string printer_name = backend->GetDefaultPrinterName();
  return base::UTF8ToUTF16(printer_name);
}