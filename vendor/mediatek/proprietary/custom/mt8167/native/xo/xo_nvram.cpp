#include <unistd.h>
#include <fcntl.h>
#include <utils/Log.h>
#include <utils/String8.h>
#include "libnvram.h"
#include "CFG_PRODUCT_INFO_File.h"
#include "Custom_NvRam_LID.h"

#define LOG_TAG "XoNvramDrv"
#define XO_DEV_PATH	"/sys/devices/platform/soc/10210000.xo/xo_board_offset"

#define NVRAM_DRV_LOG(fmt, arg...)	ALOGD(LOG_TAG " " fmt, ##arg)
#define NVRAM_DRV_ERR(fmt, arg...)	ALOGE(LOG_TAG "MError: %5d: " fmt, __LINE__, ##arg)

int main(int argc __unused, char** argv)
{
	NVRAM_DRV_ERR("Read cap from nvram for xo!\n");
	F_ID rNvramFileID;
	PRODUCT_INFO proinfo;
	int file_lid = AP_CFG_REEB_PRODUCT_INFO_LID;
	int i4RecSize = 0;
	int i4RecNum = 0;
	int ret;
	int fd_xo;
	int xo_offset;
	int flag;
	char buf[4];

	rNvramFileID = NVM_GetFileDesc(file_lid, &i4RecSize, &i4RecNum, true);
	if (rNvramFileID.iFileDesc == -1) {
		NVRAM_DRV_ERR("Open PRODUCT_INFO fail!\n");
		return -1;
	}

	if (i4RecNum != 1) {
		NVRAM_DRV_ERR("Unexpected record num %d\n", i4RecNum);
		NVM_CloseFileDesc(rNvramFileID);
		return -1;
	}

	NVRAM_DRV_ERR("PRODUCT_INFO rec_num %d, rec_size %d\n", i4RecNum, i4RecSize);

	ret = read(rNvramFileID.iFileDesc, &proinfo, sizeof(PRODUCT_INFO));
	if(ret < 0)
	{
		NVRAM_DRV_ERR("Read PRODUCT_INFO fails %d\n", errno);
		NVM_CloseFileDesc(rNvramFileID);
		return -1;
	}

	if(proinfo.xocap_flag != 0x1){
		NVRAM_DRV_ERR("xocap_flag is %d!\n", proinfo.xocap_flag);
		return 0;
	}
	xo_offset = (proinfo.xocap_offset & 0x7f);
	NVRAM_DRV_ERR("reserved %x\n", xo_offset);

	sprintf(buf, "%d", xo_offset);
	fd_xo = open(XO_DEV_PATH, O_RDWR);
	ret = write(fd_xo, buf, strlen(buf));

	if(ret < 0)
	{
		NVRAM_DRV_ERR("write failed and ret is %d\n", ret);
		return ret;
	}

	if(!NVM_CloseFileDesc(rNvramFileID)) {
		NVRAM_DRV_ERR("Close product info error!");
		return -1;
	}

	NVRAM_DRV_LOG("XO SET CAP  EXIT!");
	return 0;
}
