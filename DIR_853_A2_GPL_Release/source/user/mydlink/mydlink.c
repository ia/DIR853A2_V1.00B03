
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdarg.h>


#include "nvram.h"

#include "mdb.h"




int factoryreset_main(int argc, char **argv)
{
	system("killall -USR2 nvram_daemon");
	
	return 0;
}

/* This define is from trunk/Uboot/include/image.h */
#define IH_MAGIC	0x27051956	/* Image Magic Number		*/
#define IH_NMLEN	(32-4)		/* Image Name Length		*/
#define PRODUCT_LEN  64
#define VERSION_LEN 16
typedef struct image_header {
	unsigned int	ih_magic;	/* Image Header Magic Number	*/
	unsigned int	ih_hcrc;	/* Image Header CRC Checksum	*/
	unsigned int	ih_time;	/* Image Creation Timestamp	*/
	unsigned int	ih_size;	/* Image Data Size		*/
	unsigned int	ih_load;	/* Data	 Load  Address		*/
	unsigned int	ih_ep;		/* Entry Point Address		*/
	unsigned int	ih_dcrc;	/* Image Data CRC Checksum	*/
	unsigned char		ih_os;		/* Operating System		*/
	unsigned char		ih_arch;	/* CPU architecture		*/
	unsigned char		ih_type;	/* Image Type			*/
	unsigned char		ih_comp;	/* Compression Type		*/
	unsigned char		ih_name[IH_NMLEN];	/* Image Name		*/
	unsigned int	ih_ksz;		/* Kernel Part Size		*/
	unsigned char     product[PRODUCT_LEN];
	unsigned char     sw_version[VERSION_LEN];
	unsigned char     hw_version[VERSION_LEN];
} image_header_t;

static int mydlink_check_fw_header(const char *fw_path)
{
    unsigned char *buf_fw = 0;
    unsigned int udSize = 0;
    image_header_t *kernel_header = 0;
    unsigned int kernel_size = 0;
    unsigned long hcrc = 0;
    unsigned long hcrc_read = 0;
    unsigned long dcrc = 0;
    int ret = 0; // default Fail
    unsigned int hcrc_bkup = 0;
    
    FILE *file = fopen(fw_path, "rb");
    if (file == 0)
        goto end;

    buf_fw = (unsigned char*)malloc(16*1024*1024);
    if (buf_fw == 0)
        goto end;
        
    fseek(file, 0, SEEK_END);
    udSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    fread(buf_fw, udSize, 1, file);

    kernel_header = (image_header_t*)(buf_fw);
	kernel_size = ntohl(kernel_header->ih_size);

    // check magic number
    if ( IH_MAGIC != ntohl(kernel_header->ih_magic) )
        goto end;

    // check header crc
    hcrc_bkup = kernel_header->ih_hcrc;
    hcrc_read = ntohl(kernel_header->ih_hcrc); // backup hcrc
    kernel_header->ih_hcrc = 0;
    if (crc32( 0, buf_fw, sizeof(image_header_t) ) != hcrc_read)
        goto end;
    //kernel_header->ih_hcrc = hcrc_bkup; // if you want to write buf_fw, this step should not be ignored

    // check data crc
    if( crc32 (0, buf_fw + sizeof(image_header_t), kernel_size) != ntohl(kernel_header->ih_dcrc) )
        goto end;

    ret = 1; // Pass

end:
    if (file != 0) {fclose(file);}
    if (buf_fw != 0) {free(buf_fw);}
    return ret;
}



int fw_upgrade_main(int argc, char **argv)
{
	int ret = -1;
	if (argc != 2)
	{
		printf("usage: fw_upgrade fw.bin\n");
		return ret;
	}
	if(mydlink_check_fw_header(argv[1]))
	{
		char cmdstr[255];
		memset((void *)cmdstr, 0, sizeof(cmdstr));
		snprintf(cmdstr, sizeof(cmdstr), "mtd_write -r -w write %s Kernel &", argv[1]);
		system(cmdstr);
		//system("sleep 3; reboot;");
	}
	else
	{
		printf("invalid firmware.\n");
	}
	return ret;
}



int mdb_main(int argc, char **argv)
{
	MDB_LIST *l;
	int method = -1;
	int ret = MDB_SUCC;
	
	
	if (argc < 2)
	{
		printf("usage: mdb <set | get | apply> <attribute> <value>\n");
		return ERR_BAD_CMD;
	}
	else if (argc == 2)
	{
		if (strcmp(argv[1], "--list") == 0)
			list_cmds();
	}
	else 
	{
		l = find_mdbcmd(argv[2]);
		if (l == NULL)
		{
			printf("Don't support this mdb command: %s\n", argv[2]);
			
			return ERR_UNKNOWN_ATTR;
		}
		
		if (strcmp(argv[1], "get") == 0 && l->get)
			ret = l->get(argv[2], NULL, 0);
		else if (strcmp(argv[1], "set") == 0 && argc == 4 && l->set)
			ret = l->set(argv[2], argv[3]);
		else if (strcmp(argv[1], "apply") == 0)
			ret = mdb_apply();
	}
	
	return ret;
}
#define MYNUM_OFFSET 0xE120
#define MTD_FACTORY 	"Factory"

int mtd_open(const char *mtd, int flags)
{
	FILE *fp;
	char dev[PATH_MAX];
	char part_name[256];
	int i;
	int ret;

	sprintf(part_name, "\"%s\"",mtd);
	if ((fp = fopen("/proc/mtd", "r"))) {
		while (fgets(dev, sizeof(dev), fp)) {
			if (sscanf(dev, "mtd%d:", &i) && strstr(dev, part_name)) {
				snprintf(dev, sizeof(dev), "/dev/mtd/%d", i);
				if ((ret=open(dev, flags))<0) {
					snprintf(dev, sizeof(dev), "/dev/mtd%d", i);
					ret=open(dev, flags);
				}
				fclose(fp);
				return ret;
			}
		}
		fclose(fp);
	}

	return open(mtd, flags);
}

int mynumber_main(int argc, char **argv)
{
	int fd;
	char mynumstr[10];
	int mynum_len = 8;

	memset((void *)mynumstr, 0, sizeof(mynumstr));
	fd = mtd_open(MTD_FACTORY, O_RDWR | O_SYNC);
	if (fd < 0)
	{
		printf("read mydlink number failed\n");
		return -1;
	}
	lseek(fd, MYNUM_OFFSET, SEEK_SET);
	if (read(fd, mynumstr, mynum_len) != mynum_len)
	{
		printf("read mydlink number failed.\n");
		close(fd);
		return -1;
	}
	close(fd);
	printf("%s\n", mynumstr);
	return 0;
}

typedef struct {
	const char *name;
	int (*main)(int argc, char *argv[]);
} applets_t;


static const applets_t applets[] = {
	{ "factory_reset",		factoryreset_main },
	{ "fw_upgrade",         fw_upgrade_main   },
	{ "mdb",                mdb_main },	
	{ "mydlinknumber",      mynumber_main},
	
	{NULL, NULL}
};

int
main(int argc, char **argv)
{
	char *base = strrchr(argv[0], '/');

	base = base ? base + 1 : argv[0];

	const applets_t *a;
	for (a = applets; a->name; ++a) 
	{
		if (strcmp(base, a->name) == 0) 
		{
			return a->main(argc, argv);
		}
   	}
	return 0;
}



