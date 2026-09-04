#include <kernel.h>
#include <string.h>

#include <def_mods.h>
#include <dbgprintf.h>

#include <usbhdfsd-common.h>
#include <hdd-ioctl.h>
#define NEWLIB_PORT_AWARE
#include <fileXio_rpc.h>
#include <fileio.h>
#include <io_common.h>

#include <macros.h>

#define started_from(device) (strstr(path, device) == path)

static const char hddarg[] = "-o" "\0" "4" "\0" "-n" "\0" "20";
static const char pfsarg[] = "-m" "\0" "4" "\0" "-o" "\0" "10" "\0" "-n" "\0" "40";

static const int ds34pads = 1;

bool HDD_USABLE = false;

int check_hdd_usability() {
	int ID, ret;
	int HDDSTAT; // IOCTL...

	sleep(1); // Introduce delay to prevent ps2hdd module from hanging

	HDDSTAT = fileXioDevctl("hdd0:", HDIOC_STATUS, NULL, 0, NULL, 0); /* 0 = HDD connected and formatted, 1 = not formatted, 2 = HDD not usable, 3 = HDD not connected. */
	dbgprintf("%s: HDD status is %d\n", __func__, HDDSTAT);
	HDD_USABLE = (HDDSTAT == 0 || HDDSTAT == 1); // ONLY if HDD is usable. as we will offer HDD Formatting operation

	return !HDD_USABLE;
}

uint8_t no_dependencies[4] = {
	EMPTY_ENTRY, 
	EMPTY_ENTRY,
	EMPTY_ENTRY,
	EMPTY_ENTRY
};

#define iop_deps(a, b, c, d) ((uint8_t []) { a, b, c, d })
#define iop_dependency(dep) iop_deps(dep->id, EMPTY_ENTRY, EMPTY_ENTRY, EMPTY_ENTRY)

void register_iop_modules() {
    module_entry *iomanX_entry = 
		iopman_register_module_buffer("iomanX", iomanX, no_dependencies, NULL, NULL);
	module_entry *fileXio_entry = 
		iopman_register_module_buffer("fileXio", fileXio, iop_dependency(iomanX_entry), fileXioInit, fileXioExit);

	module_entry *sio2man_entry = 
		iopman_register_module_buffer("sio2man", sio2man, iop_dependency(fileXio_entry), NULL, NULL);

	module_entry *mcman_entry = 
		iopman_register_module_buffer("mcman", mcman, iop_dependency(sio2man_entry), NULL, NULL);
	module_entry *mcserv_entry = 
		iopman_register_module_buffer("mcserv", mcserv, iop_dependency(mcman_entry), lambda(void, 
			(module_entry *mod) { 
				if (mod->started) mcInit(MC_TYPE_XMC); 
			}), 
			NULL);
	iopman_start_module_at_boot(mcserv_entry);

	module_entry *padman_entry = 
		iopman_register_module_buffer("padman", padman, iop_dependency(sio2man_entry), pad_init, padEnd);
	iopman_start_module_at_boot(padman_entry);

	module_entry *cdfs_entry = 
		iopman_register_module_buffer("cdfs", cdfs, iop_dependency(fileXio_entry), NULL, NULL);
	iopman_start_module_at_boot(cdfs_entry);

	module_entry *usbd_entry = 
		iopman_register_module_buffer("usbd", usbd, no_dependencies, NULL, NULL);

	module_entry *bdm_entry = 
		iopman_register_module_buffer("bdm", bdm, iop_dependency(fileXio_entry), NULL, NULL);

	module_entry *bdmfs_fatfs_entry = 
		iopman_register_module_buffer("bdmfs_fatfs", bdmfs_fatfs, iop_dependency(bdm_entry), NULL, NULL);

	iopman_register_module_buffer("usbmass_bd", usbmass_bd, iop_deps(bdmfs_fatfs_entry->id, usbd_entry->id, EMPTY_ENTRY, EMPTY_ENTRY), NULL, NULL);
	
	iopman_register_module_buffer("poweroff", poweroff, no_dependencies, NULL, NULL);

	iopman_register_module_buffer("freeram", freeram, no_dependencies, NULL, NULL);
}

void prepare_IOP() {
    dbgprintf("AthenaEnv: Starting IOP Reset...\n");
    SifInitRpc(0);
    #if defined(RESET_IOP)
    while (!SifIopReset("", 0)){};
    #endif
    while (!SifIopSync()){};
    SifInitRpc(0);
    dbgprintf("AthenaEnv: IOP reset done.\n");

    // install sbv patch fix
    dbgprintf("AthenaEnv: Installing SBV Patches...\n");
    sbv_patch_enable_lmb();
    sbv_patch_disable_prefix_check();
}

bool wait_device(char *path) {
    dbgprintf("waiting for '%s'\n", path);
    struct stat buffer;
    int ret = -1;
    int retries = 500;

    while(ret != 0 && retries > 0) {
        ret = stat(path, &buffer);
        /* Wait untill the device is ready */
        nopdelay();

        retries--;
    }

    return ret == 0;
}

char *get_boot_device(const char* path) {
	char * device = NULL;

	if(started_from("mass")) {
		device = "bdm";
	} else if(started_from("mc")) {
		device = "mcserv";
	} else if(started_from("mmce")) {
		device = "mmceman";
	} else if(started_from("cdfs") || started_from("cdrom")) {
		device = "cdfs";
	} else if(started_from("hdd")) {
		device = "ps2fs";
	}

	return device;
}

char *get_block_device(const char* path) {
	char massdev[7] = { 0 };
	strncpy(massdev, path, 6);
	int fd = fileXioDopen(massdev);
	if (fd >= 0) {
		char dev_name[10];
		if (fileXioIoctl2(fd, USBMASS_IOCTL_GET_DRIVERNAME, NULL, 0, dev_name, sizeof(dev_name) - 1) >= 0) {
			fileXioDclose(fd);

			if (!strncmp(dev_name, "usb", 3)) {
				return "usbmass_bd";
			} else if (!strncmp(dev_name, "sdc", 3)) {
				return "mx4sio_bd";
			} else if (!strncmp(dev_name, "sd", 2)) {
				return "IEEE1394_bd";
			} else if (!strncmp(dev_name, "ata", 3)) {
				return "ata_bd";
			} else if (!strncmp(dev_name, "udp", 3)) {
				return "smap_udpbd";
			}
		}
	}

	return NULL;
}
