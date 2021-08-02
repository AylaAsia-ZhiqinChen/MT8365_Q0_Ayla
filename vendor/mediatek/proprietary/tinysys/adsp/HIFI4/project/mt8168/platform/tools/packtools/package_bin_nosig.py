#!/usr/bin/env python
import os, sys, string, struct, array, shutil

#Common function
def getFileSize(file):
    statinfo = os.stat(file)
    size = statinfo.st_size
    return size

def merge(src, dst):
    size1 = getFileSize(src)
    size2 = getFileSize(dst)
    fdst = open(dst, 'ab')
    fsrc = open(src, 'rb')

    while True:
        data = fsrc.read(16*1024)
        if not data :
            break;
        fdst.write(data)

    fsrc.close()
    fdst.close()
    print ("%s:%s + %s:%s -> %s:%s" %(src, size1, dst, size2, dst, getFileSize(dst)))
    return True

#IMG_BIN_INF_1.bin <-> "hifi4_iram0.bin"
def genImgBinInf1():
    ImgInf_file1 = open(python_path + "IMG_BIN_INF_1.bin", "w")
    #IMG_BIN_INF_1
    #IMG_INF[0]bit0:indicate if the image is first 1 bootloader(BL1).
    #IMG_INF[0]bit1:indicate if enabling authentication or not.
    #IMG_INF[0]bit2:Indicate if enabling encryption or not.
    #IMG_INF[0]bit3~bit7:reserved.
    ImgInf_file1.write(struct.pack("b",0))#IMG_INF[0]
    ImgInf_file1.write(struct.pack("b",0))#IMG_INF[1]//for reserved
    ImgInf_file1.write(struct.pack("b",0))#IMG_INF[2]//for reserved
    ImgInf_file1.write(struct.pack("b",0))#IMG_INF[3]//for reserved
    #Offset not include Xip_code_48k, only from BIN_MAGIC field.
    #0x00000814 = 8(BIN_MAGIC)+4(BIN_TOTAL_SZ)+4(IMG_BIN_INF_TB_SZ)+0x800(IMG_BIN_INF_TB)+4(IMG_BINS_SZ) = Total_HEAD_len
    ImgInf_file1.write(struct.pack("I",(0)))#IMG_BIN_OFST (=Total_HEAD_len=0x00000814)
    ImgInf_file1.write(struct.pack("I",getFileSize(hifi4dsp_iram0_bin)))#IMG_BIN_SZ (=IMG_SZ)
    ImgInf_file1.write(struct.pack("I",getFileSize(hifi4dsp_iram0_bin)))#IMG_SZ
    ImgInf_file1.write(struct.pack("I",hifi4dsp_iram0_bin_load_base))#LD_ADR, for test boot at I-TCM
    ImgInf_file1.close()

#IMG_BIN_INF_2.bin <-> "hifi4_dram0.bin"
def genImgBinInf2():
    ImgInf_file2 = open(python_path + "IMG_BIN_INF_2.bin", "w")
    #IMG_BIN_INF_2
    #IMG_INF[0]bit0:indicate if the image is first 1 bootloader(BL1).
    #IMG_INF[0]bit1:indicate if enabling authentication or not.
    #IMG_INF[0]bit2:Indicate if enabling encryption or not.
    #IMG_INF[0]bit3~bit7:reserved.
    ImgInf_file2.write(struct.pack("b",0))#IMG_INF[0]
    ImgInf_file2.write(struct.pack("b",0))#IMG_INF[1]//for reserved
    ImgInf_file2.write(struct.pack("b",0))#IMG_INF[2]//for reserved
    ImgInf_file2.write(struct.pack("b",0))#IMG_INF[3]//for reserved
    #Offset not include Xip_code_48k
    ImgInf_file2.write(struct.pack("I",(0 + getFileSize(hifi4dsp_iram0_bin))))#Total_HEAD_len+IMG_BIN1's size
    ImgInf_file2.write(struct.pack("I",getFileSize(hifi4dsp_dram0_bin)))#IMG_BIN_SZ
    ImgInf_file2.write(struct.pack("I",getFileSize(hifi4dsp_dram0_bin)))#IMG_SZ
    ImgInf_file2.write(struct.pack("I",hifi4dsp_dram0_bin_load_base))#LD_ADR, for test boot at D-TCM
    ImgInf_file2.close()

#IMG_BIN_INF_3.bin <-> "hifi4_dram.bin"
def genImgBinInf3():
    ImgInf_file3 = open(python_path + "IMG_BIN_INF_3.bin", "w")
    #IMG_BIN_INF_3
    #IMG_INF[0]bit0:indicate if the image is first 1 bootloader(BL1).
    #IMG_INF[0]bit1:indicate if enabling authentication or not.
    #IMG_INF[0]bit2:Indicate if enabling encryption or not.
    #IMG_INF[0]bit3~bit7:reserved.
    ImgInf_file3.write(struct.pack("b",0))#IMG_INF[0]
    ImgInf_file3.write(struct.pack("b",0))#IMG_INF[1]//for reserved
    ImgInf_file3.write(struct.pack("b",0))#IMG_INF[2]//for reserved
    ImgInf_file3.write(struct.pack("b",0))#IMG_INF[3]//for reserved
    #Offset not include Xip_code_48k
    ImgInf_file3.write(struct.pack("I",(0 + getFileSize(hifi4dsp_iram0_bin) + getFileSize(hifi4dsp_dram0_bin))))#Total_HEAD_len+IMG_BIN1's size+IMG_BIN2's size
    ImgInf_file3.write(struct.pack("I",getFileSize(hifi4dsp_dram_bin)))#IMG_BIN_SZ
    ImgInf_file3.write(struct.pack("I",getFileSize(hifi4dsp_dram_bin)))#IMG_SZ
    ImgInf_file3.write(struct.pack("I",hifi4dsp_dram_bin_load_base))#LD_ADR, for test boot at Dram
    ImgInf_file3.close()

def genImgBinInfTB():
    genImgBinInf1()
    genImgBinInf2()
    genImgBinInf3()

    total_img_bin_inf_sz = getFileSize(python_path + "IMG_BIN_INF_1.bin") + getFileSize(python_path + "IMG_BIN_INF_2.bin") + getFileSize(python_path + "IMG_BIN_INF_3.bin")

    Tab_file2 = open(python_path + "IMG_BIN_INF_TB_PART2.bin", "w")
    Tab_file2.write(struct.pack("I",core_number))#BOOT_ADR_NO(M)
    Tab_file2.write(struct.pack("I",hifi4dsp_bootup_vector_base))#DSP_1_ADR from DSP bootup entry address
    Tab_file2.write(struct.pack("I",(bin_section_number)))#IMG_BIN_INF_NO(N)
    Tab_file2.write(struct.pack("I",total_img_bin_inf_sz))#IMG_BIN_INF_SZ
    Tab_file2.close()

    merge(python_path + "IMG_BIN_INF_1.bin", python_path + "IMG_BIN_INF_TB_PART2.bin")
    merge(python_path + "IMG_BIN_INF_2.bin", python_path + "IMG_BIN_INF_TB_PART2.bin")
    merge(python_path + "IMG_BIN_INF_3.bin", python_path + "IMG_BIN_INF_TB_PART2.bin")

    #Padding for IMG_BIN_INF_TB size = 0x7F4 byte
    while getFileSize(python_path + "IMG_BIN_INF_TB_PART2.bin") < 0x7F4:
        Tab_file2 = open(python_path + "IMG_BIN_INF_TB_PART2.bin", "ab+")
        Tab_file2.write(struct.pack("I",0xFFFFFFFF))#PADDING
        Tab_file2.close()

    Tab_file1 = open(python_path + "IMG_BIN_INF_TB_PART1.bin", "w")
    Tab_file1.write(struct.pack("b",0))#TB_INF[0] version of IMG_BIN_INF_TB
    Tab_file1.write(struct.pack("b",0))#TB_INF[1] Bit0: indicate if enabling authentication(1) or not(0) for IMG_BIN_INF_TB, Bit1~7:Reserved
    Tab_file1.write(struct.pack("b",0))#TB_INF[2] Reserved
    Tab_file1.write(struct.pack("b",0))#TB_INF[3] Reserved
    Tab_file1.write(struct.pack("b",0))#TB_INF[4] Reserved
    Tab_file1.write(struct.pack("b",0))#TB_INF[5] NO Encryption      Encryption type for IMG_BIN
    Tab_file1.write(struct.pack("b",0))#TB_INF[6] No Authentication  Authentication type for IMG_BIN
    Tab_file1.write(struct.pack("b",0))#TB_INF[7] No Authentication  Authentication type for IMG_BIN_INF_TB
    Tab_file1.write(struct.pack("I",0x00000000))#TB_LD_ADR -> load to dram buffer
	#TB_SIG field none(TB_SIG_SZ = 0)
    Tab_file1.close()

    Tab_file = open(python_path + "IMG_BIN_INF_TB.bin", "w")
    Tab_file.close()
    merge(python_path + "IMG_BIN_INF_TB_PART1.bin", python_path + "IMG_BIN_INF_TB.bin")
    merge(python_path + "IMG_BIN_INF_TB_PART2.bin", python_path + "IMG_BIN_INF_TB.bin")

    os.unlink(python_path + "IMG_BIN_INF_TB_PART1.bin") #delete file
    os.unlink(python_path + "IMG_BIN_INF_TB_PART2.bin") #delete file
    return

#
#---------------------******---Python main function entry---******--------------------------------
#
core_number = 1
build_out_bin_path = sys.argv[1]
build_out_bin_name = sys.argv[2]
python_path = build_out_bin_path + "/"
output_load_name = build_out_bin_name
hifi4dsp_iram0_bin = sys.argv[3]
hifi4dsp_dram0_bin = sys.argv[4]
hifi4dsp_dram_bin  = sys.argv[5]
bin_section_number = 3
hifi4dsp_iram0_bin_load_base = int(sys.argv[6], 16)
hifi4dsp_dram0_bin_load_base = int(sys.argv[7], 16)
#
# The shared DRAM physical base(reserved) for CPU view
# may be different from DSP memory view.
# LD_ADR = DSP-view-addr(for DSP package),
# LD_ADR: bit0 indicates if the image is the DRAM section,
# example - MT8168DSP : 0x40020001
# running = CPU-reservd-addr(for CPU loading binary).
#
hifi4dsp_dram_bin_load_base  = int(sys.argv[8], 16)
hifi4dsp_bootup_vector_base  = int(sys.argv[9], 16)

#
#check input parameters
#
if core_number == 2:
    print "Package : dual core"
elif core_number == 1:
    print "Package : single core"
else:
    print "Error core number!"
    exit(1)
if os.path.exists(hifi4dsp_iram0_bin):
    pass
else:
    print "Maybe build %s error!" %(hifi4dsp_iram0_bin)
    exit(1)

#
#step1, generate IMG_BIN_INF_TB.bin
#
genImgBinInfTB()

#
#step2, pack all image header info
#
total_dsp_bin_file_size = getFileSize(hifi4dsp_iram0_bin) + getFileSize(hifi4dsp_dram0_bin) + getFileSize(hifi4dsp_dram_bin)
print "total_dsp_bin_file_size =", total_dsp_bin_file_size

myfile = open(python_path + "LOAD_NOSIG.image", "w")
myfile.close()
myfile = open(python_path + "LOAD_NOSIG.image", "ab+")
#fill BIN_MAGIC 0x4D544B445350494D BIG ENDIAN "MTKDSPIM"
myfile.write(struct.pack("I",0x444B544D)) #BIN_MAGIC
myfile.write(struct.pack("I",0x4D495053)) #BIN_MAGIC
#BIN_TOTAL_SZ not include the head (BIN_MAGIC+BIN_TOTAL_SZ) 8bytes
myfile.write(struct.pack("i",0x04 + getFileSize(python_path + "IMG_BIN_INF_TB.bin") + 0x04 + total_dsp_bin_file_size))#BIN_TOTAL_SZ
myfile.write(struct.pack("i",getFileSize(python_path + "IMG_BIN_INF_TB.bin"))) #IMG_BIN_INF_TB_SZ
myfile.close()
merge(python_path + "IMG_BIN_INF_TB.bin", python_path + "LOAD_NOSIG.image")#IMG_BIN_INF_TB
myfile = open(python_path + "LOAD_NOSIG.image", "ab+")
myfile.write(struct.pack("I",total_dsp_bin_file_size))#IMG_BINS_SZ
myfile.close()

#
#step3, pack all phase binarys into LOAD_NOSIG.image
#
merge(hifi4dsp_iram0_bin, python_path + "LOAD_NOSIG.image")
merge(hifi4dsp_dram0_bin, python_path + "LOAD_NOSIG.image")
merge(hifi4dsp_dram_bin,  python_path + "LOAD_NOSIG.image")

#
#step4, Copy binary and clean
#
os.unlink(python_path + "IMG_BIN_INF_1.bin")  #delete file
os.unlink(python_path + "IMG_BIN_INF_2.bin")  #delete file
os.unlink(python_path + "IMG_BIN_INF_3.bin")  #delete file
os.unlink(python_path + "IMG_BIN_INF_TB.bin") #delete file

shutil.move(python_path+"LOAD_NOSIG.image", output_load_name)
print "%s" %(output_load_name)
print "Package image done, total size : ", getFileSize(output_load_name)
