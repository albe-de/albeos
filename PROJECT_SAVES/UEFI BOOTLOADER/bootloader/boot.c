/* ha! how ya like that, linux-elf! Able, OOUTT */
// Pure UEFI Test - boot.c

typedef unsigned long long UINTN;
typedef unsigned short CHAR16;
typedef void* EFI_HANDLE;
typedef unsigned long long EFI_STATUS;

typedef struct SIMPLE_TEXT_OUTPUT_INTERFACE {
    void* _buf[3];
    EFI_STATUS (*OutputString)(struct SIMPLE_TEXT_OUTPUT_INTERFACE*, CHAR16*);
    EFI_STATUS (*ClearScreen)(struct SIMPLE_TEXT_OUTPUT_INTERFACE*);
} SIMPLE_TEXT_OUTPUT_INTERFACE;

typedef struct {
    void* _buf;
    SIMPLE_TEXT_OUTPUT_INTERFACE* ConOut;
} EFI_SYSTEM_TABLE;

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
    CHAR16* msg = L"UEFI TEST: Hello World!\r\n";
    SystemTable->ConOut->OutputString(SystemTable->ConOut, msg);

    while (1);
    return 0;
}
