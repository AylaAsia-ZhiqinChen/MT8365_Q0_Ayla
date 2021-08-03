/*
 * Copyright (C) 2019 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 */

#include <linux/random.h>
#include <gz-trusty/trusty.h>
#include <gz-trusty/smcall.h>

ssize_t vmm_fast_add_show(struct device *dev, struct device_attribute *attr,
	char *buf)
{
	s32 a, b, c, ret;

	get_random_bytes(&a, sizeof(s32));
	a &= 0xFF;
	get_random_bytes(&b, sizeof(s32));
	b &= 0xFF;
	get_random_bytes(&c, sizeof(s32));
	c &= 0xFF;
	ret = trusty_fast_call32(dev, MTEE_SMCNR(SMCF_FC_TEST_ADD, dev),
				 a, b, c);
	return scnprintf(buf, PAGE_SIZE, "%d + %d + %d = %d, %s\n", a, b, c,
			 ret, (a + b + c) == ret ? "PASS" : "FAIL");
}

static ssize_t vmm_fast_add_store(struct device *dev,
	struct device_attribute *attr, const char *buf,
	size_t n)
{
	return n;
}

DEVICE_ATTR_RW(vmm_fast_add);


ssize_t vmm_fast_multiply_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	s32 a, b, c, ret;

	get_random_bytes(&a, sizeof(s32));
	a &= 0xFF;
	get_random_bytes(&b, sizeof(s32));
	b &= 0xFF;
	get_random_bytes(&c, sizeof(s32));
	c &= 0xFF;
	ret = trusty_fast_call32(dev, MTEE_SMCNR(SMCF_FC_TEST_MULTIPLY, dev),
				 a, b, c);
	return scnprintf(buf, PAGE_SIZE, "%d * %d * %d = %d, %s\n", a, b, c,
			 ret, (a * b * c) == ret ? "PASS" : "FAIL");
}

static ssize_t vmm_fast_multiply_store(struct device *dev,
	struct device_attribute *attr, const char *buf,
	size_t n)
{
	return n;
}

DEVICE_ATTR_RW(vmm_fast_multiply);

ssize_t vmm_std_add_show(struct device *dev, struct device_attribute *attr,
	char *buf)
{
	s32 a, b, c, ret;

	get_random_bytes(&a, sizeof(s32));
	a &= 0xFF;
	get_random_bytes(&b, sizeof(s32));
	b &= 0xFF;
	get_random_bytes(&c, sizeof(s32));
	c &= 0xFF;
	ret = trusty_std_call32(dev, MTEE_SMCNR(SMCF_SC_TEST_ADD, dev),
						a, b, c);
	return scnprintf(buf, PAGE_SIZE, "%d + %d + %d = %d, %s\n", a, b, c,
			 ret, (a + b + c) == ret ? "PASS" : "FAIL");
}

static ssize_t vmm_std_add_store(struct device *dev,
	struct device_attribute *attr, const char *buf,
	size_t n)
{
	return n;
}

DEVICE_ATTR_RW(vmm_std_add);

ssize_t vmm_std_multiply_show(struct device *dev, struct device_attribute *attr,
	char *buf)
{
	s32 a, b, c, ret;

	get_random_bytes(&a, sizeof(s32));
	a &= 0xFF;
	get_random_bytes(&b, sizeof(s32));
	b &= 0xFF;
	get_random_bytes(&c, sizeof(s32));
	c &= 0xFF;
	ret = trusty_std_call32(dev, MTEE_SMCNR(SMCF_SC_TEST_MULTIPLY, dev),
				a, b, c);
	return scnprintf(buf, PAGE_SIZE, "%d * %d * %d = %d, %s\n", a, b, c,
			 ret, (a * b * c) == ret ? "PASS" : "FAIL");
}

static ssize_t vmm_std_multiply_store(struct device *dev,
	struct device_attribute *attr, const char *buf,
	size_t n)
{
	return n;
}

DEVICE_ATTR_RW(vmm_std_multiply);

void trusty_create_debugfs_vmm(struct trusty_state *s, struct device *pdev)
{
	int ret;

	ret = device_create_file(pdev, &dev_attr_vmm_fast_add);
	if (ret)
		goto err_create_vmm_fast_add;

	ret = device_create_file(pdev, &dev_attr_vmm_fast_multiply);
	if (ret)
		goto err_create_vmm_fast_multiply;

	ret = device_create_file(pdev, &dev_attr_vmm_std_add);
	if (ret)
		goto err_create_vmm_std_add;

	ret = device_create_file(pdev, &dev_attr_vmm_std_multiply);
	if (ret)
		goto err_create_vmm_std_multiply;

	return;

err_create_vmm_std_multiply:
	device_remove_file(pdev, &dev_attr_vmm_std_multiply);
err_create_vmm_std_add:
	device_remove_file(pdev, &dev_attr_vmm_std_add);
err_create_vmm_fast_multiply:
	device_remove_file(pdev, &dev_attr_vmm_fast_multiply);
err_create_vmm_fast_add:
	device_remove_file(pdev, &dev_attr_vmm_fast_add);
}
