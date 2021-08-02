// Customer ID=13943; Build=0x75f5e; Copyright (c) 2008-2013 Tensilica Inc.  ALL RIGHTS RESERVED.
// 
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.


// Instructions: Include this file in your xtsc-run script like this
//   #include "<PutPathHere>/extra.proxy.IO.inc"
//   --set_proxy_parm=extra_inputs=$(EXTRA_INPUTS)
//   --set_proxy_parm=extra_outputs=$(EXTRA_OUTPUTS)
//   --create_proxy=my_proxy

#define EXTRA_INPUTS=CLK,1,DCacheADataB0,64,DCacheADataB1,64,DCacheBDataB0,64,DCacheBDataB1,64,DCacheCDataB0,64,DCacheCDataB1,64,DCacheDDataB0,64,DCacheDDataB1,64,DTagAData0,23,DTagBData0,23,DTagCData0,23,DTagDData0,23,DTagAData1,23,DTagBData1,23,DTagCData1,23,DTagDData1,23,TIE_ERI_RD_In,32,TIE_ERI_RD_Rdy,1,TIE_ERI_WR_In,1,ICacheAData,128,ICacheBData,128,ICacheCData,128,ICacheDData,128,ITagAData,22,ITagBData,22,ITagCData,22,ITagDData,22,DebugInstr,88,DebugMode,1,EnableRunStall,1,ExecDebugInstr,1,InterruptAllConds,1,OCDDbgInt,1,OCDEnabled_W,1,OCDOverride,1,OCDRSRBus_M,32,PerfMonInt,1,PrefetchRamData,128,TMode,1,TModeClkGateOverride,1,PDebugEnable,1
#define EXTRA_OUTPUTS=DCacheAEnB0,1,DCacheAEnB1,1,DCacheAWrB0,1,DCacheAWrB1,1,DCacheAddrB0,9,DCacheAddrB1,9,DCacheBEnB0,1,DCacheBEnB1,1,DCacheBWrB0,1,DCacheBWrB1,1,DCacheByteEnB0,8,DCacheByteEnB1,8,DCacheCEnB0,1,DCacheCEnB1,1,DCacheCWrB0,1,DCacheCWrB1,1,DCacheDEnB0,1,DCacheDEnB1,1,DCacheDWrB0,1,DCacheDWrB1,1,DCacheWrDataB0,64,DCacheWrDataB1,64,DTagAEn0,1,DTagAWr0,1,DTagAddr0,6,DTagBEn0,1,DTagBWr0,1,DTagCEn0,1,DTagCWr0,1,DTagDEn0,1,DTagDWr0,1,DTagWrData0,23,DTagAEn1,1,DTagAWr1,1,DTagAddr1,6,DTagBEn1,1,DTagBWr1,1,DTagCEn1,1,DTagCWr1,1,DTagDEn1,1,DTagDWr1,1,DTagWrData1,23,TIE_ERI_RD_Out,14,TIE_ERI_RD_Out_Req,1,TIE_ERI_WR_Out,46,TIE_ERI_WR_Out_Req,1,EncodedExceptionVector_W,5,ICacheAEn,1,ICacheAWordEn,4,ICacheAWr,1,ICacheAddr,9,ICacheBEn,1,ICacheBWordEn,4,ICacheBWr,1,ICacheCEn,1,ICacheCWordEn,4,ICacheCWr,1,ICacheDEn,1,ICacheDWordEn,4,ICacheDWr,1,ICacheWrData,128,ITagAEn,1,ITagAWr,1,ITagAddr,6,ITagBEn,1,ITagBWr,1,ITagCEn,1,ITagCWr,1,ITagDEn,1,ITagDWr,1,ITagWrData,22,GlobalStall,1,InOCDMode_ps_C4,1,Interrupt4_1hot_EXC,1,PSCurIntLvl_W,4,RFDD_W,1,RFDO_W,1,RealExcept_W,1,ReceivedRunStall,1,SRAddr_W,8,SRRead_W,1,SRWrite_W,1,Valid_W,1,WSRBus_W,32,PrefetchRamAddr,7,PrefetchRamEn,1,PrefetchRamPIFWEn,2,PrefetchRamWr,1,PrefetchRamWrData,128,PDebugData,32,PDebugInbPif,8,PDebugInst,32,PDebugLS0Addr,32,PDebugLS0Data,32,PDebugLS0Stat,32,PDebugLS1Addr,32,PDebugLS1Data,32,PDebugLS1Stat,32,PDebugOutPif,8,PDebugPC,32,PDebugPrefetchL1Fill,4,PDebugPrefetchLookup,8,PDebugStatus,8
