#include <fcntl.h>
#include <unistd.h>
#include <utils/Log.h>
#include <utils/String8.h>
#include <cutils/properties.h>
#include "libnvram.h"
#include "libfile_op.h"
#include "CFG_DcxoCap_File.h"
#include "Custom_NvRam_LID.h"

#define LOG_TAG		"DcxoNvramDrv"
#define MT6357_DCXO_BOARD_OFFSET_PATH	\
	"/sys/devices/platform/soc/1000d000.pwrap/1000d000.pwrap:mt6357-pmic/mt6357-dcxo/dcxo_board_offset"
#define MT6357_NVRAM_BOARD_OFFSET_PATH	\
	"/sys/devices/platform/soc/1000d000.pwrap/1000d000.pwrap:mt6357-pmic/mt6357-dcxo/nvram_board_offset"

#define NVRAM_DRV_LOG(fmt, arg...)	ALOGD(LOG_TAG " " fmt, ##arg)
#define NVRAM_DRV_ERR(fmt, arg...)	\
	ALOGE(LOG_TAG "MError: %5d: " fmt, __LINE__, ##arg)

#define DCXO_TOTAL_TOTAL_MASK		0xff

int load_nvram_to_dcxo_board_offset(void)
{
	F_ID dcxo_nvram_fd;
	File_DcxoCap_Struct dcxoinfo;
	int file_lid = AP_CFG_CUSTOM_FILE_DCXOCAP_LID;
	int i4RecSize = 0;
	int i4RecNum = 0;
	int ret;
	int fd_dcxo;
	int dcxo_offset;
	char buf[5];

	NVRAM_DRV_ERR("%s: Set cap from nvram to dcxo_board_offset\n", __func__);

	dcxo_nvram_fd = NVM_GetFileDesc(file_lid, &i4RecSize, &i4RecNum, ISREAD);
	if (dcxo_nvram_fd.iFileDesc == -1) {
		NVRAM_DRV_ERR("Open nvram DcxoCap failed!\n");
		return -1;
	}

	if (i4RecNum != 1) {
		NVRAM_DRV_ERR("Unexpected record num %d\n", i4RecNum);
		NVM_CloseFileDesc(dcxo_nvram_fd);
		return -1;
	}

	NVRAM_DRV_ERR("DcxoCap rec_num %d, rec_size %d\n", i4RecNum, i4RecSize);

	ret = read(dcxo_nvram_fd.iFileDesc, &dcxoinfo, sizeof(File_DcxoCap_Struct));
	if(ret < 0)
	{
		NVRAM_DRV_ERR("Read DcxoCapOffset from nvram failed. ret=%d\n", ret);
		NVM_CloseFileDesc(dcxo_nvram_fd);
		return -1;
	}

	dcxo_offset = (dcxoinfo.dcxocap_offset & DCXO_TOTAL_TOTAL_MASK);
	NVRAM_DRV_ERR("nvram data: 0x%x\n", dcxo_offset);

	sprintf(buf, "%d", dcxo_offset);
	fd_dcxo = open(MT6357_DCXO_BOARD_OFFSET_PATH, O_RDWR);
	ret = write(fd_dcxo, buf, strlen(buf));
	if(ret < 0)
	{
		NVRAM_DRV_ERR("Write dcxo_board_offset failed. errno=%d\n", errno);
		return ret;
	}
	close(fd_dcxo);

	if(!NVM_CloseFileDesc(dcxo_nvram_fd)) {
		NVRAM_DRV_ERR("Close product info error!\n");
		return -1;
	}

	NVRAM_DRV_ERR("DCXO SET CAP EXIT!\n");
	return 0;
}

int save_nvram_board_offset_to_nvram(void)
{
	F_ID dcxo_nvram_fd;
	File_DcxoCap_Struct dcxoinfo;
	int file_lid = AP_CFG_CUSTOM_FILE_DCXOCAP_LID;
	int i4RecSize = 0;
	int i4RecNum = 0;
	int ret;
	int fd_dcxo;
	int dcxo_offset;
	char buf[5];

	NVRAM_DRV_ERR("%s: Save cap from nvram_board_offset to nvram\n", __func__);

	dcxo_nvram_fd = NVM_GetFileDesc(file_lid, &i4RecSize, &i4RecNum, ISWRITE);
	if (dcxo_nvram_fd.iFileDesc == -1) {
		NVRAM_DRV_ERR("Open nvram DcxoCap failed!\n");
		return -1;
	}

	if (i4RecNum != 1) {
		NVRAM_DRV_ERR("Unexpected record num %d\n", i4RecNum);
		NVM_CloseFileDesc(dcxo_nvram_fd);
		return -1;
	}

	NVRAM_DRV_ERR("DcxoCap rec_num %d, rec_size %d\n", i4RecNum, i4RecSize);

	fd_dcxo = open(MT6357_NVRAM_BOARD_OFFSET_PATH, O_RDWR);
	ret = read(fd_dcxo, buf, 5);
	if(ret < 0)
	{
		NVRAM_DRV_ERR("Read nvram_board_offset failed. ret=%d\n", ret);
		return ret;
	}

	sscanf(buf, "0x%x", &dcxo_offset);
	dcxoinfo.dcxocap_offset = (dcxo_offset & DCXO_TOTAL_TOTAL_MASK);
	NVRAM_DRV_ERR("nvram_board_offset: 0x%x\n", dcxoinfo.dcxocap_offset);

	ret = write(dcxo_nvram_fd.iFileDesc, &dcxoinfo, sizeof(File_DcxoCap_Struct));
	if(ret < 0)
	{
		NVRAM_DRV_ERR("Write DcxoCap to nvram failed. errno=%d\n", errno);
		NVM_CloseFileDesc(dcxo_nvram_fd);
		return -1;
	}

	if(!NVM_CloseFileDesc(dcxo_nvram_fd)) {
		NVRAM_DRV_ERR("Close product info error!\n");
		return -1;
	}

	if (!FileOp_BackupToBinRegion_All())
	{
		NVRAM_DRV_ERR("Failed FileOp_BackupToBinRegion_All!\n");
		return -1;
	}

	close(fd_dcxo);

	NVRAM_DRV_ERR("DCXO SAVE NVRAM CAP EXIT!\n");
	return 0;
}

int load_nvram_to_nvram_board_offset(void)
{
	F_ID dcxo_nvram_fd;
	File_DcxoCap_Struct dcxoinfo;
	int file_lid = AP_CFG_CUSTOM_FILE_DCXOCAP_LID;
	int i4RecSize = 0;
	int i4RecNum = 0;
	int ret;
	int fd_dcxo;
	int dcxo_offset;
	char buf[5];

	NVRAM_DRV_ERR("%s: Load cap from nvram to nvram_board_offset\n", __func__);

	dcxo_nvram_fd = NVM_GetFileDesc(file_lid, &i4RecSize, &i4RecNum, ISREAD);
	if (dcxo_nvram_fd.iFileDesc == -1) {
		NVRAM_DRV_ERR("Open nvram DcxoCap failed!\n");
		return -1;
	}

	if (i4RecNum != 1) {
		NVRAM_DRV_ERR("Unexpected record num %d\n", i4RecNum);
		NVM_CloseFileDesc(dcxo_nvram_fd);
		return -1;
	}

	NVRAM_DRV_ERR("DcxoCap rec_num %d, rec_size %d\n", i4RecNum, i4RecSize);

	ret = read(dcxo_nvram_fd.iFileDesc, &dcxoinfo, sizeof(File_DcxoCap_Struct));
	if(ret < 0)
	{
		NVRAM_DRV_ERR("Read DcxoCapOffset from nvram failed. ret=%d\n", ret);
		NVM_CloseFileDesc(dcxo_nvram_fd);
		return -1;
	}

	dcxo_offset = (dcxoinfo.dcxocap_offset & DCXO_TOTAL_TOTAL_MASK);
	NVRAM_DRV_ERR("nvram data: 0x%x\n", dcxo_offset);

	sprintf(buf, "%d", dcxo_offset);
	fd_dcxo = open(MT6357_NVRAM_BOARD_OFFSET_PATH, O_RDWR);
	ret = write(fd_dcxo, buf, strlen(buf));
	if(ret < 0)
	{
		NVRAM_DRV_ERR("Write nvram_board_offset failed. errno=%d\n", errno);
		return ret;
	}
	close(fd_dcxo);

	if(!NVM_CloseFileDesc(dcxo_nvram_fd)) {
		NVRAM_DRV_ERR("Close product info error!\n");
		return -1;
	}

	NVRAM_DRV_ERR("DCXO LOAD NVRAM CAP EXIT!\n");
	return 0;
}

int main(int argc, char** argv)
{
	char nvram_init_val[PROPERTY_VALUE_MAX];
	int ret;
	int timeout = 0;

	while(1) {
		property_get("vendor.service.nvram_init", nvram_init_val, NULL);
		NVRAM_DRV_ERR("vendor.service.nvram_init=%s\n",nvram_init_val);
		if(strcmp(nvram_init_val, "Ready") != 0) {
			timeout++;
			if (timeout >= 10) {
				NVRAM_DRV_ERR("DcxoSetCap wait nvram_init timeout\n");
				break;
			}
		} else {
			break;
		}
		usleep(5000000);
	}

	if (argc == 2 && !strcmp(argv[1], "save"))
		ret = save_nvram_board_offset_to_nvram();
	else if (argc == 2 && !strcmp(argv[1], "load"))
		ret = load_nvram_to_nvram_board_offset();
	else
		ret = load_nvram_to_dcxo_board_offset();

	return ret;
}

