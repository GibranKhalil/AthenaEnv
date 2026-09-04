#ifndef DEF_MODS_H
#define DEF_MODS_H

#include <sifrpc.h>
#include <loadfile.h>
#include <libmc.h>
#include <iopheap.h>
#include <iopcontrol.h>
#include <smod.h>
#include <sbv_patches.h>
#include <smem.h>
#include <libpwroff.h>

#include <iop_manager.h>

extern bool HDD_USABLE;

iopman_define_module(iomanX);
iopman_define_module(fileXio);
iopman_define_module(sio2man);
iopman_define_module(mcman);
iopman_define_module(mcserv);
iopman_define_module(padman);
iopman_define_module(cdfs);
iopman_define_module(usbd);
iopman_define_module(bdm);
iopman_define_module(bdmfs_fatfs);
iopman_define_module(usbmass_bd);
iopman_define_module(poweroff);
iopman_define_module(freeram);

void register_iop_modules();
char *get_boot_device(const char* path);
char *get_block_device(const char* path);
int load_default_module(int id);
bool wait_device(char *path);
void prepare_IOP();

#endif
