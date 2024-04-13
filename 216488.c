bool IsDeviceNameValid(const base::string16& device_name) {
#if defined(OS_MACOSX)
  base::ScopedCFTypeRef<CFStringRef> new_printer_id(
      base::SysUTF16ToCFStringRef(device_name));
  PMPrinter new_printer = PMPrinterCreateFromPrinterID(new_printer_id.get());
  bool printer_exists = new_printer != nullptr;
  PMRelease(new_printer);
  return printer_exists;
#elif defined(OS_WIN)
  printing::ScopedPrinterHandle printer;
  return printer.OpenPrinterWithName(device_name.c_str());
#endif
  return true;
}