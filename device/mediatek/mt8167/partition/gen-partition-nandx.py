#!/usr/bin/env python

from __future__ import print_function
import binascii
import json
import os
import struct
import sys
import uuid
import xml.dom.minidom

def write(path, data):
	with open(path, "wb") as f:
		f.write(data)

def write_scatter_partition(f, entry):
	if entry.get("file_name", "NONE") != "NONE":
		entry.setdefault("is_download", True)
		entry.setdefault("is_upgradable", True)
		entry.setdefault("operation_type", "UPDATE")
		entry.setdefault("type", "NORMAL_ROM")
		if entry["file_type"] != "Raw data":
			entry.setdefault("type", "FTL20_IMG")
	f.write(
"""- partition_index: %s
  partition_name: %s
  file_name: %s
  is_download: %s
  type: %s
  linear_start_addr: %s
  physical_start_addr: %s
  partition_size: %s
  region: %s
  storage: %s
  boundary_check: %s
  is_reserved: %s
  operation_type: %s
  d_type: %s
  slc_percentage: %s
  is_upgradable: %s
  reserve: %s

""" % (entry["partition_index"], entry["partition_name"], entry.get("file_name", "NONE"),
entry.get("is_download", False) and "true" or "false",
entry.get("type", "NONE"), hex(entry["linear_start_addr"]),
hex(entry["physical_start_addr"]), hex(entry["partition_size"] * 1024), entry.get("region", "NONE"),
entry.get("storage", "HW_STORAGE_NAND"), entry.get("boundary_check", True) and "true" or "false",
entry.get("is_reserved", False) and "true" or "false", entry.get("operation_type", "PROTECTED"),
entry["d_type"], entry.get("slc_percentage", "0"), entry.get("is_upgradable", False) and "true" or "false",
hex(entry.get("reserve", 0))))

def gen_partition(f, partition, d, gpt_xml, gpt_mk):
	f.write(
"""############################################################################################################
#
#  General Setting
#
############################################################################################################
- general: MTK_PLATFORM_CFG
  info:
    - config_version: V1.1.7
      platform: %s
      project: %s
      storage: NAND
      boot_channel: NONE
      block_size: 0x20000
############################################################################################################
#
#  Layout Setting
#
############################################################################################################
""" % (PLATFORM, PROJECT))
	d["PRELOADER"]["partition_index"] = "SYS0"
	if os.getenv("MTK_AB_OTA_UPDATER") == "yes":
		d["PRELOADER"]["is_upgradable"] = "true"
	d["PRELOADER"]["file_name"] = "preloader_"+PROJECT+".bin"
	write_scatter_partition(f, d["PRELOADER"])
	i = 1
	total = d["PRELOADER"]["partition_size"]
	mk_file = open(gpt_mk, 'w+')
	mk_file.write(str("BOARD_FLASH_PAGE_SIZE:=16384\n"))
	xml_file = open(gpt_xml, 'w+')
	mntl_start_sector = 4
	mntl_partition_file = {'ANDROID' : 'system.img', 'VENDOR' : 'vendor.img', 'CACHE' : 'cache.img', 'USRDATA' : 'userdata.img','SUPER' : 'super.img'}
	mntl_partition_name = {'ANDROID' : 'mntlblk_d1', 'VENDOR' : 'mntlblk_d2', 'CACHE' : 'cache', 'USRDATA' : 'userdata', 'SUPER' : 'mntlblk_d1'}
	data_part_fake_size = 2 * 1024 * 1024; # 2GB
	for node in partition.childNodes:
		if node.nodeName != "entry":
			continue
		name = node.getAttribute("name")
		if name not in d:
			continue
		start = node.getAttribute("start")
		start = start and eval(start) or 0
		end = node.getAttribute("end")
		end = end and eval(end) or 0
		size = node.getAttribute("size")
		size = size and eval(size) or 0
		entry = d[name]
		if size != 0 and entry["file_type"] != "Mntl data":
			start = total
			total += size
		elif end != start:
			size = end + 1 - start
		else:
			size = 0
		if name == "USRDATA":
			start = total
		entry["partition_name"] = name
		entry["partition_index"] = "SYS%d" % i
		entry["linear_start_addr"] = start * 1024
		entry["physical_start_addr"] = start * 1024
		entry["partition_size"] = size
		if name == "BMTPOOL":
			entry["linear_start_addr"] = 0xffff0000
			entry["physical_start_addr"] = 0xffff0000
		if entry["file_type"] == "Mntl data":
			partition_size = node.getAttribute("size")
			partition_size = partition_size and eval(partition_size) or 0
			if entry["partition_name"] == "USRDATA":
				partition_size = data_part_fake_size
			partition_sector_num = partition_size * 1024 / 4096
			mntl_end_sector = mntl_start_sector + partition_sector_num - 1
			xml_file.write(str("\t<entry type=\"{0FC63DAF-8483-4772-8E79-3D69D8477DE4}\""))
			xml_file.write(str(" start=\"")+str(mntl_start_sector)+str("\" end=\"")+str(mntl_end_sector)+str("\" name=\"")+str(mntl_partition_name[name])+str("\" file_name=\"")+str(mntl_partition_file[name]))
			xml_file.write(str("\" />\n"))
			size_byte = partition_size * 1024
			if name == "ANDROID":
				name = "SYSTEM"
			if name == "USRDATA":
				name = "USERDATA"
			mk_file.write(str("BOARD_")+str(name)+str("IMAGE_START_SECTOR:=")+str(mntl_start_sector)+str("\n"))
			mntl_start_sector += partition_sector_num
			if entry["partition_name"] != "USRDATA":
				continue
			entry["file_name"] = "mntl.img"
			entry["type"] = "FTL20_IMG"
		write_scatter_partition(f, entry)
		i += 1
	total_lba = mntl_end_sector + 4
	xml_file.seek(0, 0)
	old = xml_file.read()
	xml_file.seek(0, 0)
	xml_file.write('<?xml version=\"1.0\" encoding=\"utf-8\"?>\n')
	xml_file.write(str("<partition lba=\"")+str(total_lba)+str("\">\n"))
	xml_file.write(old)
	xml_file.write("</partition>\n")
	xml_file.close()
	mk_file.write(str("BOARD_TOTAL_SECTORS:=")+str(total_lba))
	mk_file.close()

PLATFORM = ""
PROJECT  = ""
def main(argv):
	global PLATFORM
	global PROJECT
	PLATFORM = argv[5]
	PROJECT  = argv[6]
	if len(argv) != 7 and len(argv) != 8:
		print("Usage: %s partition.xml MBR [scatter.txt]" % argv[0])
		exit(1)
	root = xml.dom.minidom.parse(argv[1])
	for partition in root.childNodes:
		if partition.nodeName == "partition":
			break
		else:
			raise Exception("partition not found")
	with open(argv[2], "r") as f:
		d = json.load(f)
	with open(argv[4], "w") as f:
		gen_partition(f, partition, d, argv[3], argv[7])
	return 0

if __name__ == "__main__":
	sys.exit(main(sys.argv))
