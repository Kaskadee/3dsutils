// Copyright 2014 Citra Emulator Project
// Licensed under GPLv2
// Refer to the license.txt file included.

#include <cstdio>

#include <3ds.h>

#include "fs_common.h"
#include "utils/shared_font/shared_font.h"

#define SYSDATA_PATH "/3dsutils/sysdata/"

namespace SharedFont {

static const u32 SHARED_FONT_SIZE = 0x300000;

void Dump() {
    if (!CreateFullPath(SYSDATA_PATH)) {
        printf("Creating path (%s) failed! Aborting!\n", SYSDATA_PATH);
        return;
    }
    std::string path = SYSDATA_PATH "/shared_font.bin";
    SanitizeSeparators(&path);

    printf("Dumping shared system font (%s)... ", path.c_str());

    // Connect to APT service...

    Handle apt_handle;
    srvGetServiceHandle(&apt_handle, "APT:U");
    u32* cmdbuf = getThreadCommandBuffer();

    // Call APT::GetSharedFont function to load font into memory...

    cmdbuf[0] = 0x00440000;
    svcSendSyncRequest(apt_handle);

    Handle mem_handle = cmdbuf[4];
    void* shared_font_addr = reinterpret_cast<void*>(cmdbuf[2]);

    // Close APT handle...

    svcCloseHandle(apt_handle);

    // Map shared font memory...

    svcMapMemoryBlock(mem_handle, 0, MEMPERM_READ, MEMPERM_DONTCARE);

    // Dump shared font to SDMC...

    FILE* out_file = fopen(path.c_str(), "wb");
    size_t bytes_written = fwrite(shared_font_addr, 1, SHARED_FONT_SIZE, out_file);
    fclose(out_file);

    if (bytes_written == SHARED_FONT_SIZE)
        printf("Done!\n");
    else
        printf("Failed!\n");
}

} // namespace
