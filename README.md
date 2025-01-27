# WW Pak Bypass

A modern C++ implementation of [shorekeeper-patch](https://github.com/thexeondev/shorekeeper-patch) for OS_2.0.2, designed to bypass pak file verification.

## Requirements

- Visual Studio 2019 or later
- Windows SDK 10.0 and above
- C++17 or later

## Building from source

1. Clone the repository with submodules:

```bash
git clone --recurse-submodules https://github.com/hellokittyfan48/ww-pak-bypass.git
```

2. Open the solution file `pak-bypass.sln` in Visual Studio
3. Compile in `Release | x64`
4. Inject & Start

## Find signature

Search for "Checking pak file "%s". This may take a while..." in a disassembler

Or find this using cheat engine `40 55 56 57 41 57 48 8d 6c 24 ? 48 81 ec ? ? ? ? 80 3d`

**Or make an actual signature:**

```asm
Client-Win64-Shipping.exe+3FF8E50 - 40 55                 - push rbp  --------------------- We scaninng for this
Client-Win64-Shipping.exe+3FF8E52 - 56                    - push rsi
Client-Win64-Shipping.exe+3FF8E53 - 57                    - push rdi
Client-Win64-Shipping.exe+3FF8E54 - 41 57                 - push r15
Client-Win64-Shipping.exe+3FF8E56 - 48 8D 6C 24 C1        - lea rbp,[rsp-3F]
Client-Win64-Shipping.exe+3FF8E5B - 48 81 EC E8000000     - sub rsp,000000E8
Client-Win64-Shipping.exe+3FF8E62 - 80 3D 5F464004 04     - cmp byte ptr [Client-Win64-Shipping.exe+83FD4C8],04
Client-Win64-Shipping.exe+3FF8E69 - 48 8D 3D 58385202     - lea rdi,[Client-Win64-Shipping.exe+651C6C8]
Client-Win64-Shipping.exe+3FF8E70 - 0F29 BC 24 B0000000   - movaps [rsp+000000B0],xmm7
Client-Win64-Shipping.exe+3FF8E78 - 48 8B F1              - mov rsi,rcx
Client-Win64-Shipping.exe+3FF8E7B - 72 36                 - jb Client-Win64-Shipping.exe+3FF8EB3
Client-Win64-Shipping.exe+3FF8E7D - 83 79 10 00           - cmp dword ptr [rcx+10],00
Client-Win64-Shipping.exe+3FF8E81 - 74 06                 - je Client-Win64-Shipping.exe+3FF8E89
```

## Need help?

- You can join my Discord [here](https://hellokittyfan48.github.io/)

### Leave a 🌟 if you like it <3
