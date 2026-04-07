# ChromiumDecryptor

A stealth DLL for decrypting Chromium-based browser master keys using COM-based elevation and direct syscalls.

![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![License](https://img.shields.io/badge/license-MIT-green?style=for-the-badge)
![Windows](https://img.shields.io/badge/Platform-Windows-blue?style=for-the-badge&logo=windows&logoColor=white)

> [!CAUTION]
> **Disclaimer:** This project is for educational purposes only. Use this software at your own risk. I am not responsible for any damage or misuse of this tool.

---

### Features

- **COM Elevation:** Uses IElevator COM interface to decrypt app_bound_encrypted_key
- **Direct Syscalls:** Bypasses AV/EDR hooks using syscall stubs with SSN resolution
- **Multi-Browser Support:** Chrome, Chrome Beta, Brave, Edge, Avast
- **Edge ASTER Key:** Extracts aster_app_bound_encrypted_key for Microsoft Edge
- **Manual Mapping:** Self-injects via reflective loading to avoid detection
- **Key Storage:** Saves decrypted keys to `chromium_keys.txt`

### Technical Details

- **Elevation Method:** Browser COM component (IElevatorChrome, IEdgeElevator2Final)
- **Syscall Resolution:** Hash-based function name matching with SSN derivation
- **Supported Browsers:** Chrome, Brave, Edge, Avast Browser
- **x64 Only:** Targets 64-bit processes

### Quick Start

1. **Build:**
   - Open `.slnx` in **Visual Studio 2022**, select **Release x64**, build
   - Output: `ChromiumDecryptor.dll`

2. **Inject:**
   - Manual map the DLL into target browser process (chrome.exe, msedge.exe, brave.exe, etc.)
   - Keys will be saved to `chromium_keys.txt` in the DLL's directory

### Project Structure

```
ChromiumDecryptor/
├── Main.cpp              — Entry point, key extraction logic
├── Elevator.cpp/.h       — COM elevation and decryption
├── Methods.cpp/.h        — Syscall initialization and stubs
├── Bootstrap.cpp/.h      — Reflective loader / manual map
├── Settings.h            — Browser configurations and CLSIDs
├── Common.h              — Common utilities
└── Syscalls.asm          — Syscall trampoline assembly
```

---

Created by [nullptrflow](https://github.com/nullptrflow)
