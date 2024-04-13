void CjfifDecode::SetStatusText(CString strText)
{
	// Make sure that we have been connected to the status
	// bar of the main window first! Note that it is jpegsnoopDoc
	// that sets this variable.
	if (m_pStatBar) {
		m_pStatBar->SetPaneText(0,strText);
	}
}