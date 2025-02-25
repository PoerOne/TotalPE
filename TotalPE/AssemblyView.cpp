#include "pch.h"
#include "AssemblyView.h"
#include "..\External\Capstone\capstone.h"
#include "PEStrings.h"

#ifdef _DEBUG
#pragma comment(lib, "../External/Capstone/Capstoned.lib")
#else
#pragma comment(lib, "../External/Capstone/Capstone.lib")
#endif

bool CAssemblyView::SetCode(uint64_t startAddress, std::vector<uint8_t> const& code) {
    csh handle;
    if (cs_open(CS_ARCH_X86, PE().get_image().is_x32_image() ? CS_MODE_32 : CS_MODE_64, &handle) != CS_ERR_OK)
        return false;
    auto bytes = code.data();
    auto size = code.size();
    cs_insn inst{};
    CString text;
    while (cs_disasm_iter(handle, &bytes, &size, &startAddress, &inst)) {
        text += PEStrings::FormatInstruction(inst) + L"\r\n";
        if (_strcmpi(inst.mnemonic, "ret") == 0)
            break;
    }

    m_Edit.SetWindowText(text);
    cs_close(&handle);
    return true;
}

LRESULT CAssemblyView::OnCreate(UINT, WPARAM, LPARAM, BOOL&) {
    m_hWndClient = m_Edit.Create(m_hWnd, rcDefault, L"",
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_VSCROLL | WS_HSCROLL |
        ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL | ES_AUTOHSCROLL, WS_EX_CLIENTEDGE);

    CFont font;
    font.CreatePointFont(110, L"Consolas");
    m_Edit.SetFont(font.Detach());
    m_Edit.SetTabStops(10);

    return 0;
}

LRESULT CAssemblyView::OnCopy(WORD, WORD, HWND, BOOL&) {
    return LRESULT();
}
