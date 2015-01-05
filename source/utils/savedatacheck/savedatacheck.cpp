// Copyright 2014 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <memory>
#include <cstdlib>
#include <cstdio>

#include <3ds.h>

#include "output.h"
#include "utils/shared_font/shared_font.h"

namespace SaveDataCheck {
    
std::string BuildSharedRomFSFilename(u8* lowpath) {
    const u32* split_lowpath = reinterpret_cast<u32*>(lowpath);
    char* filename_buffer;
    asprintf(&filename_buffer, "/%08x%08x.bin", split_lowpath[1], split_lowpath[0]);
    std::string filename(filename_buffer);
    free(filename_buffer);
    return filename;
}
    
void DumpSharedRomFS(u8* archive_binary_lowpath) {    
    std::string output_file = BuildSharedRomFSFilename(archive_binary_lowpath);
    
    // Read RomFS bin from SaveDataCheck...
    
    Handle romfs_handle;
    u64    romfs_size        = 0;
    u32    romfs_bytes_read  = 0;
    
    FS_archive savedatacheck_archive    = { 0x2345678a, { PATH_BINARY, 16, archive_binary_lowpath } };
    u8         file_binary_lowpath[20]  = {};
    FS_path    romfs_path               = { PATH_BINARY, 20, file_binary_lowpath };
    
    print(GFX_TOP, "Dumping SaveDataCheck RomFS (%s)... ", output_file.c_str());

    FSUSER_OpenFileDirectly(NULL, &romfs_handle, savedatacheck_archive, romfs_path, FS_OPEN_READ, FS_ATTRIBUTE_NONE);
    FSFILE_GetSize(romfs_handle, &romfs_size);
    
    std::unique_ptr<u8> romfs_data_buffer(new u8[romfs_size]);
    FSFILE_Read(romfs_handle, &romfs_bytes_read, 0, romfs_data_buffer.get(), romfs_size);
    FSFILE_Close(romfs_handle);
    
    // Dump RomFS bin to SDMC...
    
    Handle     file_handle;
    u32        bytes_written = 0;
    FS_path    fs_path       = FS_makePath(PATH_CHAR, output_file.c_str());
    FS_archive sdmc_archive  = (FS_archive) { 0x00000009, { PATH_EMPTY, 1, (u8*) "" } };
    
    FSUSER_OpenArchive(NULL, &sdmc_archive);
    FSUSER_OpenFile(NULL, &file_handle, sdmc_archive, fs_path, FS_OPEN_CREATE | FS_OPEN_WRITE, FS_ATTRIBUTE_NONE);
    Result res = FSFILE_Write(file_handle, &bytes_written, 0x0, romfs_data_buffer.get(), romfs_size, FS_WRITE_FLUSH);
    FSFILE_Close(file_handle);
    FSUSER_CloseArchive(NULL, &sdmc_archive);
    
    // Check result...
    
    if (res == 0 && bytes_written == romfs_size)
        print(GFX_TOP, "Done!\n");
    else
        print(GFX_TOP, "Failed!\n");
}

void Dump() {
    // savedatacheck/000400db00010302.bin
    u8 binary_lowpath_000400db00010302[] = { 
        0x02, 0x03, 0x01, 0x00, 0xdb, 0x00, 0x04, 0x00, 0x00, 0xff, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00
    };
    DumpSharedRomFS(binary_lowpath_000400db00010302);
    
    // savedatacheck/0004009b00010202.bin
    u8 binary_lowpath_0004009b00010202[] = { 
        0x02, 0x02, 0x01, 0x00, 0x9b, 0x00, 0x04, 0x00, 0x00, 0xff, 0xff, 0xff, 0x02, 0x00, 0x00, 0x00
    };
    DumpSharedRomFS(binary_lowpath_0004009b00010202);
    
    // savedatacheck/0004009b00010402.bin
    u8 binary_lowpath_0004009b00010402[] = { 
        0x02, 0x04, 0x01, 0x00, 0x9b, 0x00, 0x04, 0x00, 0x00, 0xff, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00
    };
    DumpSharedRomFS(binary_lowpath_0004009b00010402);
}

} // namespace
