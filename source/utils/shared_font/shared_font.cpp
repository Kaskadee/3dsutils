// Copyright 2014 Citra Emulator Project
// Licensed under GPLv2
// Refer to the license.txt file included.

#include <cstdio>

#include <3ds.h>

#include "output.h"
#include "utils/shared_font/shared_font.h"

namespace SharedFont {

static const u32 SHARED_FONT_SIZE = 0x300000;

void Dump() {
    static const char* path = "/shared_font.bin";

    print(GFX_TOP, "Dumping shared system font (%s)... ", path);

    // Connect to APT service...

    Handle apt_handle;
    srvGetServiceHandle(&apt_handle, "APT:U");
    u32* cmdbuf=getThreadCommandBuffer();

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

    FILE* out_file = fopen(path, "wb");
    size_t bytes_written = fwrite(shared_font_addr, 1, SHARED_FONT_SIZE, out_file);
    fclose(out_file);

    // Check result...

    if (bytes_written == SHARED_FONT_SIZE)
        print(GFX_TOP, "Done!\n");
    else
        print(GFX_TOP, "Failed!\n");
}

} // namespace
