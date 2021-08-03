/*
 * Copyright (C) 2016 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/types.h>

#include "trustzone/kree/system.h"
#include "trustzone/kree/mem.h"
#include "tz_cross/ta_vpu.h"
#include "tz_cross/trustzone.h"
#include "vpu_cmn.h"

KREE_SESSION_HANDLE vpu_session;

#if 0
static int vpu_sec_allocate_shared_mem(struct vpu_sec_ctx *ctx, u32 size)
{
	int ret;

	CMDQ_MSG("enter %s, tee=%p, size=%d\n", __func__, tee, size);
	ctx->shared_mem.size = size;
	ctx->shared_mem.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT;
	ret = TEEC_AllocateSharedMemory(&ctx->context,
		&ctx->shared_mem);
	if (ret != TEEC_SUCCESS) {
		LOG_ERR("%s: failed to allocate shared mem: 0x%x\n", __func__,
			ret);
	} else {
		LOG_INF("%s: allocate shared mem: 0x%x, pshared_mem:0x%p\n",
			__func__, ret, ctx->shared_mem.buffer);
	}

	return ret;
}

static void vpu_sec_free_shared_mem(struct TEEC_SharedMemory *mem)
{
	if (mem)
		TEEC_ReleaseSharedMemory(mem);
}
#endif

static int vpu_sec_sdsp_lock(struct vpu_device *vpu_device)
{
	int ret = 0, i;
	struct vpu_core *vpu_core = NULL;

	LOG_WRN("SDSP_SEC_LOCK mutex in\n");

	if (vpu_device->in_sec_world)
		return 0;

	for (i = 0 ; i < vpu_device->core_num ; i++) {
		vpu_core = vpu_device->vpu_core[i];
		mutex_lock(&vpu_core->sdsp_control_mutex);
	}

	LOG_WRN("SDSP_SEC_LOCK mutex-m lock\n");
	ret = vpu_sdsp_get_power(vpu_device);
	LOG_WRN("SDSP_POWER_ON %s\n", ret == 0?"done":"fail");
	if (!vpu_is_available(vpu_device)) {
		LOG_ERR("vpu_queue is not empty!!\n");
		return -EPERM;
	}
	/* Disable IRQ */
	vpu_device->in_sec_world = true;

	return ret;
}

static void vpu_sec_sdsp_unlock(struct vpu_device *vpu_device)
{
	int ret, i;
	struct vpu_core *vpu_core = NULL;

	if (!vpu_device->in_sec_world)
		return;

	ret = vpu_sdsp_put_power(vpu_device);
	LOG_WRN("DSP_SEC_UNLOCK %s\n", ret == 0?"done":"fail");
	/* Enable IRQ */
	vpu_device->in_sec_world = false;

	for (i = 0 ; i < vpu_device->core_num ; i++) {
		vpu_core = vpu_device->vpu_core[i];
		//enable_irq(vpu_core->irq);
		mutex_unlock(&vpu_core->sdsp_control_mutex);
	}

	LOG_WRN("DSP_SEC_UNLOCK mutex-m unlock\n");
}

static int vpu_sec_create_session(void)
{
	int ret = 0;

	if (!vpu_session) {
		ret = KREE_CreateSession(TZ_TA_VPU_UUID, &vpu_session);
		if (ret != TZ_RESULT_SUCCESS) {
			LOG_ERR("%s: fail to create session(0x%x)\n", __func__,
			       ret);
			return ret;
		}
	}

	LOG_DBG("%s: create session(0x%x)\n", __func__, ret);

	return ret;
}

static void vpu_sec_close_session(KREE_SESSION_HANDLE session)
{
	if (session)
		KREE_CloseSession(session);
}

uint32_t vpu_sec_load(void)
{
	//union MTEEC_PARAM param[4];
	uint32_t paramTypes;
	int ret;

	paramTypes = TZPT_NONE;

	ret = KREE_TeeServiceCall(vpu_session, VPU_TZCMD_LOAD, paramTypes,
				  NULL);
	if (ret != TZ_RESULT_SUCCESS)
		LOG_ERR("%s: failed to ServiceCall 0x%x\n", __func__, ret);
	LOG_DBG("%s: KREE_TeeServiceCall end\n", __func__);

	return ret;
}

void vpu_sec_unload(void)
{
	uint32_t paramTypes;
	int ret;

	paramTypes = TZPT_NONE;

	ret = KREE_TeeServiceCall(vpu_session, VPU_TZCMD_UNLOAD, paramTypes,
				  NULL);
	if (ret != TZ_RESULT_SUCCESS)
		LOG_ERR("%s: failed to ServiceCall 0x%x\n", __func__, ret);
	LOG_DBG("%s: KREE_TeeServiceCall end\n", __func__);
}

uint32_t vpu_sec_execute(void)
{
	uint32_t paramTypes = TZPT_NONE;
	int ret;

	LOG_DBG("%s: KREE_TeeServiceCall start\n", __func__);

	ret = KREE_TeeServiceCall(vpu_session, VPU_TZCMD_EXECUTE, paramTypes,
				  NULL);
	if (ret != TZ_RESULT_SUCCESS)
		LOG_ERR("%s: failed to ServiceCall 0x%x\n", __func__, ret);
	LOG_DBG("%s: KREE_TeeServiceCall end\n", __func__);

	return ret;
}

uint32_t vpu_sec_test(void)
{
	uint32_t paramTypes;
	int ret;

	LOG_DBG("%s: start\n", __func__);

	paramTypes = TZPT_NONE;
	LOG_DBG("%s: KREE_TeeServiceCall start\n", __func__);

	ret = KREE_TeeServiceCall(vpu_session, VPU_TZCMD_TEST, paramTypes,
				  NULL);
	if (ret != TZ_RESULT_SUCCESS)
		LOG_ERR("%s: failed to ServiceCall 0x%x\n", __func__, ret);
	LOG_DBG("%s: KREE_TeeServiceCall end\n", __func__);

	return ret;
}

int vpu_set_sec_test_parameter(struct vpu_device *vpu_device, uint8_t param,
			       int argc, int *args)
{
	int ret = 0;

	switch (param) {
	case VPU_DEBUG_SEC_ATTACH:
		ret = (argc == 1) ? 0 : -EINVAL;
		if (ret) {
			LOG_ERR("invalid argument, expected:1, received:%d\n",
				argc);
			goto out;
		}
		ret = vpu_sec_sdsp_lock(vpu_device);
		if (ret) {
			LOG_ERR("failed to do sdsp lock: %d\n", ret);
			goto out;
		}

		ret = vpu_sec_create_session();
		if (ret) {
			LOG_ERR("%s: failed to vpu_sec_create_session(%d)\n",
				__func__, ret);
			goto err_sec_sdsp_unlock;
		}

		break;
	case VPU_DEBUG_SEC_LOAD:
		ret = (argc == 1) ? 0 : -EINVAL;
		if (ret) {
			LOG_ERR("invalid argument, expected:1, received:%d\n",
				argc);
			goto out;
		}
		ret = vpu_sec_load();
		if (ret) {
			LOG_ERR("%s: failed to vpu_sec_load(%x)\n",
			       __func__, ret);
			goto err_sec_close_session;
		}

		break;
	case VPU_DEBUG_SEC_EXCUTE:
		ret = (argc == 1) ? 0 : -EINVAL;
		if (ret) {
			LOG_ERR("invalid argument, expected:1, received:%d\n",
				argc);
			goto out;
		}

		ret = vpu_sec_execute();
		LOG_INF("vpu_sec_execute_algo result = %d\n",
		       ret);

		break;
	case VPU_DEBUG_SEC_UNLOAD:
		ret = (argc == 1) ? 0 : -EINVAL;
		if (ret) {
			LOG_ERR("invalid argument, expected:1, received:%d\n",
				argc);
			goto out;
		}
		vpu_sec_unload();

		break;
	case VPU_DEBUG_SEC_DETACH:
		ret = (argc == 1) ? 0 : -EINVAL;
		if (ret) {
			LOG_ERR("invalid argument, expected:1, received:%d\n",
				argc);
			goto out;
		}
		vpu_sec_close_session(vpu_session);
		vpu_session = 0;
		vpu_sec_sdsp_unlock(vpu_device);

		break;
	case VPU_DEBUG_SEC_TEST:
		ret = (argc == 1) ? 0 : -EINVAL;
		if (ret) {
			LOG_ERR("invalid argument, expected:1, received:%d\n",
				argc);
			goto out;
		}
		ret = vpu_sec_sdsp_lock(vpu_device);
		if (ret) {
			LOG_ERR("failed to do sdsp lock: %d\n", ret);
			goto out;
		}

		ret = vpu_sec_create_session();
		if (ret) {
			LOG_ERR("%s: failed to vpu_sec_create_session(%d)\n",
				__func__, ret);
			goto err_sec_sdsp_unlock;
		}

		ret = vpu_sec_test();
		if (ret) {
			LOG_ERR("%s: failed to vpu_sec_boot_up(%x)\n",
			       __func__, ret);
			goto err_sec_close_session;
		}

		vpu_sec_close_session(vpu_session);
		vpu_session = 0;
		vpu_sec_sdsp_unlock(vpu_device);

		break;
	default:
		LOG_ERR("unsupport the vpu_sec parameter:%d\n", param);
		break;
	}

	return ret;

err_sec_close_session:
	vpu_sec_close_session(vpu_session);
	vpu_session = 0;
err_sec_sdsp_unlock:
	vpu_sec_sdsp_unlock(vpu_device);
out:
	return ret;
}

