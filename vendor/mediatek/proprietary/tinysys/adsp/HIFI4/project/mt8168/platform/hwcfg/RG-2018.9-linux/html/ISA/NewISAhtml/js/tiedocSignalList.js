
var tiedocSignalList = {
    "AXI Master": {
        "ARADDR": {
            "width": "[32-1:0]",
            "dir": "output",
            "desc": "",
        },
        "ARBURST": {
            "width": "[2-1:0]",
            "dir": "output",
            "desc": "",
        },
        "ARCACHE": {
            "width": "[4-1:0]",
            "dir": "output",
            "desc": "",
        },
        "ARID": {
            "width": "[4-1:0]",
            "dir": "output",
            "desc": "",
        },
        "ARLEN": {
            "width": "[8-1:0]",
            "dir": "output",
            "desc": "",
        },
        "ARLOCK": {
            "width": "",
            "dir": "output",
            "desc": "",
        },
        "ARPROT": {
            "width": "[3-1:0]",
            "dir": "output",
            "desc": "",
        },
        "ARQOS": {
            "width": "[4-1:0]",
            "dir": "output",
            "desc": "",
        },
        "ARREADY": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "ARSIZE": {
            "width": "[3-1:0]",
            "dir": "output",
            "desc": "",
        },
        "ARVALID": {
            "width": "",
            "dir": "output",
            "desc": "",
        },
        "ARXTUSER": {
            "width": "[4-1:0]",
            "dir": "output",
            "desc": "",
        },
        "AWADDR": {
            "width": "[32-1:0]",
            "dir": "output",
            "desc": "",
        },
        "AWBURST": {
            "width": "[2-1:0]",
            "dir": "output",
            "desc": "",
        },
        "AWCACHE": {
            "width": "[4-1:0]",
            "dir": "output",
            "desc": "",
        },
        "AWID": {
            "width": "[4-1:0]",
            "dir": "output",
            "desc": "",
        },
        "AWLEN": {
            "width": "[8-1:0]",
            "dir": "output",
            "desc": "",
        },
        "AWLOCK": {
            "width": "",
            "dir": "output",
            "desc": "",
        },
        "AWPROT": {
            "width": "[3-1:0]",
            "dir": "output",
            "desc": "",
        },
        "AWQOS": {
            "width": "[4-1:0]",
            "dir": "output",
            "desc": "",
        },
        "AWREADY": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "AWSIZE": {
            "width": "[3-1:0]",
            "dir": "output",
            "desc": "",
        },
        "AWVALID": {
            "width": "",
            "dir": "output",
            "desc": "",
        },
        "AWXTUSER": {
            "width": "[4-1:0]",
            "dir": "output",
            "desc": "",
        },
        "BID": {
            "width": "[4-1:0]",
            "dir": "input",
            "desc": "",
        },
        "BREADY": {
            "width": "",
            "dir": "output",
            "desc": "",
        },
        "BRESP": {
            "width": "[2-1:0]",
            "dir": "input",
            "desc": "",
        },
        "BVALID": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "RDATA": {
            "width": "[axiwidth-1:0]",
            "dir": "input",
            "desc": "",
        },
        "RID": {
            "width": "[4-1:0]",
            "dir": "input",
            "desc": "",
        },
        "RLAST": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "RREADY": {
            "width": "",
            "dir": "output",
            "desc": "",
        },
        "RRESP": {
            "width": "[2-1:0]",
            "dir": "input",
            "desc": "",
        },
        "RVALID": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "WDATA": {
            "width": "[axiwidth-1:0]",
            "dir": "output",
            "desc": "",
        },
        "WLAST": {
            "width": "",
            "dir": "output",
            "desc": "",
        },
        "WREADY": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "WSTRB": {
            "width": "[axiwidth/8-1:0]",
            "dir": "output",
            "desc": "",
        },
        "WVALID": {
            "width": "",
            "dir": "output",
            "desc": "",
        },
    },
    "AXI Slave": {
        "ARADDR_S": {
            "width": "[32-1:0]",
            "dir": "input",
            "desc": "",
        },
        "ARBURST_S": {
            "width": "[2-1:0]",
            "dir": "input",
            "desc": "",
        },
        "ARCACHE_S": {
            "width": "[4-1:0]",
            "dir": "input",
            "desc": "",
        },
        "ARID_S": {
            "width": "[16-1:0]",
            "dir": "input",
            "desc": "",
        },
        "ARLEN_S": {
            "width": "[8-1:0]",
            "dir": "input",
            "desc": "",
        },
        "ARLOCK_S": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "ARPROT_S": {
            "width": "[3-1:0]",
            "dir": "input",
            "desc": "",
        },
        "ARQOS_S": {
            "width": "[4-1:0]",
            "dir": "input",
            "desc": "",
        },
        "ARREADY_S": {
            "width": "",
            "dir": "output",
            "desc": "",
        },
        "ARSIZE_S": {
            "width": "[3-1:0]",
            "dir": "input",
            "desc": "",
        },
        "ARVALID_S": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "AWADDR_S": {
            "width": "[32-1:0]",
            "dir": "input",
            "desc": "",
        },
        "AWBURST_S": {
            "width": "[2-1:0]",
            "dir": "input",
            "desc": "",
        },
        "AWCACHE_S": {
            "width": "[4-1:0]",
            "dir": "input",
            "desc": "",
        },
        "AWID_S": {
            "width": "[16-1:0]",
            "dir": "input",
            "desc": "",
        },
        "AWLEN_S": {
            "width": "[8-1:0]",
            "dir": "input",
            "desc": "",
        },
        "AWLOCK_S": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "AWPROT_S": {
            "width": "[3-1:0]",
            "dir": "input",
            "desc": "",
        },
        "AWQOS_S": {
            "width": "[4-1:0]",
            "dir": "input",
            "desc": "",
        },
        "AWREADY_S": {
            "width": "",
            "dir": "output",
            "desc": "",
        },
        "AWSIZE_S": {
            "width": "[3-1:0]",
            "dir": "input",
            "desc": "",
        },
        "AWVALID_S": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "BID_S": {
            "width": "[16-1:0]",
            "dir": "output",
            "desc": "",
        },
        "BREADY_S": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "BRESP_S": {
            "width": "[2-1:0]",
            "dir": "output",
            "desc": "",
        },
        "BVALID_S": {
            "width": "",
            "dir": "output",
            "desc": "",
        },
        "RDATA_S": {
            "width": "[axiwidth-1:0]",
            "dir": "output",
            "desc": "",
        },
        "RID_S": {
            "width": "[16-1:0]",
            "dir": "output",
            "desc": "",
        },
        "RLAST_S": {
            "width": "",
            "dir": "output",
            "desc": "",
        },
        "RREADY_S": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "RRESP_S": {
            "width": "[2-1:0]",
            "dir": "output",
            "desc": "",
        },
        "RVALID_S": {
            "width": "",
            "dir": "output",
            "desc": "",
        },
        "WDATA_S": {
            "width": "[axiwidth-1:0]",
            "dir": "input",
            "desc": "",
        },
        "WLAST_S": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "WREADY_S": {
            "width": "",
            "dir": "output",
            "desc": "",
        },
        "WSTRB_S": {
            "width": "[axiwidth/8-1:0]",
            "dir": "input",
            "desc": "",
        },
        "WVALID_S": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
    },
    "Access Port": {
        "DBGEN": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "JTCK": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "JTDI": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "JTDO": {
            "width": "",
            "dir": "output",
            "desc": "",
        },
        "JTDOEn": {
            "width": "",
            "dir": "output",
            "desc": "",
        },
        "JTMS": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "JTRST": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "NIDEN": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "PADDR": {
            "width": "[32-1:0]",
            "dir": "input",
            "desc": "",
        },
        "PBCLK": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "PBCLKEN": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "PENABLE": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "PRDATA": {
            "width": "[32-1:0]",
            "dir": "output",
            "desc": "",
        },
        "PREADY": {
            "width": "",
            "dir": "output",
            "desc": "",
        },
        "PRESETn": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "PSEL": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "PSLVERR": {
            "width": "",
            "dir": "output",
            "desc": "",
        },
        "PWDATA": {
            "width": "[32-1:0]",
            "dir": "input",
            "desc": "",
        },
        "PWRITE": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "SPIDEN": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "SPNIDEN": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
    },
    "AltResetVec": {
        "AltResetVec": {
            "width": "[31:0]",
            "dir": "input",
            "desc": "",
        },
    },
    "DRam0B0": {
        "DRam0BusyB0": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
    },
    "DRam0B1": {
        "DRam0BusyB1": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
    },
    "DRam1B0": {
        "DRam1BusyB0": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
    },
    "DRam1B1": {
        "DRam1BusyB1": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
    },
    "Debug": {
        "AFREADY": {
            "width": "",
            "dir": "output",
            "desc": "",
        },
        "AFVALID": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "ATBYTES": {
            "width": "[2-1:0]",
            "dir": "output",
            "desc": "",
        },
        "ATCLK": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "ATCLKEN": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "ATDATA": {
            "width": "[32-1:0]",
            "dir": "output",
            "desc": "",
        },
        "ATID": {
            "width": "[7-1:0]",
            "dir": "output",
            "desc": "",
        },
        "ATREADY": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "ATRESETn": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "ATVALID": {
            "width": "",
            "dir": "output",
            "desc": "",
        },
        "BreakIn": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "BreakInAck": {
            "width": "",
            "dir": "output",
            "desc": "",
        },
        "BreakOut": {
            "width": "",
            "dir": "output",
            "desc": "",
        },
        "BreakOutAck": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "CrossTriggerIn": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "CrossTriggerInAck": {
            "width": "",
            "dir": "output",
            "desc": "",
        },
        "CrossTriggerOut": {
            "width": "",
            "dir": "output",
            "desc": "",
        },
        "CrossTriggerOutAck": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "OCDHaltOnReset": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "TraceMemReady": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "XOCDMode": {
            "width": "",
            "dir": "output",
            "desc": "",
        },
    },
    "EXPSTATE": {
        "TIE_EXPSTATE": {
            "width": "[31:0]",
            "dir": "output",
            "desc": "",
        },
    },
    "Fault Handling": {
        "DoubleExceptionError": {
            "width": "",
            "dir": "output",
            "desc": "",
        },
        "PFatalError": {
            "width": "",
            "dir": "output",
            "desc": "",
        },
        "PFaultInfo": {
            "width": "[32-1:0]",
            "dir": "output",
            "desc": "",
        },
        "PFaultInfoValid": {
            "width": "",
            "dir": "output",
            "desc": "",
        },
    },
    "IMPWIRE": {
        "TIE_IMPWIRE": {
            "width": "[31:0]",
            "dir": "input",
            "desc": "",
        },
    },
    "IRam0": {
        "IRam0Busy": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "IRam0LoadStore": {
            "width": "",
            "dir": "output",
            "desc": "",
        },
    },
    "IRam1": {
        "IRam1Busy": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "IRam1LoadStore": {
            "width": "",
            "dir": "output",
            "desc": "",
        },
    },
    "Interrupt": {
        "BInterrupt": {
            "width": "[17:0]",
            "dir": "input",
            "desc": "",
        },
        "PWaitMode": {
            "width": "",
            "dir": "output",
            "desc": "",
        },
    },
    "MFP1": {
        "TIE_MFP1_In": {
            "width": "[31:0]",
            "dir": "input",
            "desc": "",
        },
        "TIE_MFP1_Out": {
            "width": "[34:0]",
            "dir": "output",
            "desc": "",
        },
        "TIE_MFP1_Out_Req": {
            "width": "",
            "dir": "output",
            "desc": "",
        },
    },
    "MFP2": {
        "TIE_MFP2_In": {
            "width": "[31:0]",
            "dir": "input",
            "desc": "",
        },
        "TIE_MFP2_Out": {
            "width": "[38:0]",
            "dir": "output",
            "desc": "",
        },
        "TIE_MFP2_Out_Req": {
            "width": "",
            "dir": "output",
            "desc": "",
        },
    },
    "System Signals": {
        "BReset": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "CLK": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "DReset": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "PRID": {
            "width": "[15:0]",
            "dir": "input",
            "desc": "",
        },
        "RunStall": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "StatVectorSel": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "Strobe": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "TMode": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "TModeClkGateOverride": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
    },
    "TracePort": {
        "PDebugData": {
            "width": "[31:0]",
            "dir": "output",
            "desc": "",
        },
        "PDebugEnable": {
            "width": "",
            "dir": "input",
            "desc": "",
        },
        "PDebugInbPif": {
            "width": "[7:0]",
            "dir": "output",
            "desc": "",
        },
        "PDebugInst": {
            "width": "[31:0]",
            "dir": "output",
            "desc": "",
        },
        "PDebugLS0Addr": {
            "width": "[31:0]",
            "dir": "output",
            "desc": "",
        },
        "PDebugLS0Data": {
            "width": "[31:0]",
            "dir": "output",
            "desc": "",
        },
        "PDebugLS0Stat": {
            "width": "[31:0]",
            "dir": "output",
            "desc": "",
        },
        "PDebugLS1Addr": {
            "width": "[31:0]",
            "dir": "output",
            "desc": "",
        },
        "PDebugLS1Data": {
            "width": "[31:0]",
            "dir": "output",
            "desc": "",
        },
        "PDebugLS1Stat": {
            "width": "[31:0]",
            "dir": "output",
            "desc": "",
        },
        "PDebugOutPif": {
            "width": "[7:0]",
            "dir": "output",
            "desc": "",
        },
        "PDebugPC": {
            "width": "[31:0]",
            "dir": "output",
            "desc": "",
        },
        "PDebugPrefetchL1Fill": {
            "width": "[3:0]",
            "dir": "output",
            "desc": "",
        },
        "PDebugPrefetchLookup": {
            "width": "[7:0]",
            "dir": "output",
            "desc": "",
        },
        "PDebugStatus": {
            "width": "[7:0]",
            "dir": "output",
            "desc": "",
        },
    },
    "mfp1status": {
        "TIE_mfp1status": {
            "width": "[7:0]",
            "dir": "input",
            "desc": "",
        },
    },
    "mfp2status": {
        "TIE_mfp2status": {
            "width": "[7:0]",
            "dir": "input",
            "desc": "",
        },
    },
    "qi": {
        "TIE_qi": {
            "width": "[4:0]",
            "dir": "output",
            "desc": "",
        },
    },
    "qo": {
        "TIE_qo": {
            "width": "[4:0]",
            "dir": "output",
            "desc": "",
        },
    },
    "qo_trig": {
        "TIE_qo_trig": {
            "width": "[4:0]",
            "dir": "output",
            "desc": "",
        },
    },
};


