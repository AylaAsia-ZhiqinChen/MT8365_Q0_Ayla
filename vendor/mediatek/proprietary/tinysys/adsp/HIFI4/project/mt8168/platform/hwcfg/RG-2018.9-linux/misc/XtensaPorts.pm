# Copyright © 2013-2018 by Cadence Design Systems Inc.  ALL RIGHTS RESERVED.
# These coded instructions, statements, and computer programs are the copyrighted 
# works and confidential proprietary information of Cadence Design Systems Inc.  
# They may not be modified, copied, reproduced, distributed, or disclosed to 
# third parties in any manner, medium, or form, in whole or in part, without the 
# prior written consent of Cadence Design Systems Inc.

package XtensaPorts;

use strict;
use Carp;
use Exporter;
@XtensaPorts::ISA = qw(Exporter);
@XtensaPorts::EXPORT = qw();
@XtensaPorts::EXPORT_OK = ();
@XtensaPorts::EXPORT_TAGS = ();
use vars qw(%parameters %ports);


my %buildoptions = (
    'trax'              => 1,
    'trax_log_mem_size' => 18,
);

sub getBuildOptions {
   sort keys %buildoptions;
}

sub getBuildOption {
    my $option = shift;
    die "Error: option $option doesn't exist in XtensaPorts\n" unless(exists $buildoptions{$option});
    $buildoptions{$option};
}

%parameters = (
    'maxiwidth'     => 88,
    'width'         => 32,
    'iwidth'        => 88,
    'ridx'          => 4,
    'pcwidth'       => 32,
    'awidth'        => 32,
    'eawidth'       => 32,
    'pawidth'       => 32,
    'eintrwidth'    => 18,
    'bewidth'       => 8,
    'iccntsize'     => 3,
    'icwidth'       => 128,
    'iccwidth'      => 0,
    'itwidth'       => 22,
    'itcwidth'      => 0,
    'wordenwidth'   => 4,
    'iramwidth'     => 128,
    'iramwords'     => 4,
    'iramawidth'    => 12,
    'iram0width'    => 128,
    'iram0words'    => 4,
    'iram0awidth'   => 12,
    'iram0cwidth'   => 0,
    'iram1width'    => 128,
    'iram1words'    => 4,
    'iram1awidth'   => 12,
    'iram1cwidth'   => 0,
    'iromwidth'     => 0,
    'iromwords'     => 0,
    'asize'         => 6,
    'tpdwidth'      => 32,
    'tpiwidth'      => 32,
    'tpswidth'      => 8,
    'dwaybawidth'   => 13,
    'dcwidth'       => 64,
    'nbytes'        => 8,
    'dccwidth'      => 0,
    'dtwidth'       => 23,
    'dtcwidth'      => 0,
    'dcwidthoff'    => 4,
    'dclineoffset'  => 7,
    'edrdwidth'     => 64,
    'edwrwidth'     => 64,
    'edrdbytecnt'   => 8,
    'edrdlbw'       => 3,
    'jtiwidth'      => 5,
    'cpn'           => 2,
    'iPgLsb'        => 29,
    'dPgLsb'        => 29,
    'itawidth'      => 6,
    'icsetawidth'   => 9,
    'iromawidth'    => 0,
    'wbcountwidth'  => 6,
    'reqid'         => 6,
    'dreqid'        => 6,
    'ifwayselwidth' => 3,
   );
sub getParameters {
   \%parameters;
}
# Port info structure 
# signal_name -> [ direction, bit_index, group, interface_type, protocol, delay, isolation ] 
%ports = (
    'CLK'                => [ 'input',         '', 'Global', 'xtintfGlobal', 'CLK', '', '' ],
    'BReset'             => [ 'input',         '', 'Global', 'xtintfGlobal', 'Reset', 'C*0.25', '-' ],
    'ICacheAData'        => [ 'input',  '[127:0]', 'ICache', 'xtintfInstCache', 'CacheAData', '0.09', '-' ],
    'ITagAData'          => [ 'input',   '[21:0]', 'ICache', 'xtintfInstCache', 'TagAData', '0.09', '-' ],
    'ICacheBData'        => [ 'input',  '[127:0]', 'ICache', 'xtintfInstCache', 'CacheBData', '0.09', '-' ],
    'ITagBData'          => [ 'input',   '[21:0]', 'ICache', 'xtintfInstCache', 'TagBData', '0.09', '-' ],
    'ICacheCData'        => [ 'input',  '[127:0]', 'ICache', 'xtintfInstCache', 'CacheCData', '0.09', '-' ],
    'ITagCData'          => [ 'input',   '[21:0]', 'ICache', 'xtintfInstCache', 'TagCData', '0.09', '-' ],
    'ICacheDData'        => [ 'input',  '[127:0]', 'ICache', 'xtintfInstCache', 'CacheDData', '0.09', '-' ],
    'ITagDData'          => [ 'input',   '[21:0]', 'ICache', 'xtintfInstCache', 'TagDData', '0.09', '-' ],
    'IRam0Data'          => [ 'input',  '[127:0]', 'IRam0', 'xtintfInstRam', 'Data', '0.09', 'M0' ],
    'IRam1Data'          => [ 'input',  '[127:0]', 'IRam1', 'xtintfInstRam', 'Data', '0.09', 'M0' ],
    'DTagAData1'         => [ 'input',   '[22:0]', 'DCache1', 'xtintfDataCache', 'TagAData', '0.09', '-' ],
    'DTagBData1'         => [ 'input',   '[22:0]', 'DCache1', 'xtintfDataCache', 'TagBData', '0.09', '-' ],
    'DTagCData1'         => [ 'input',   '[22:0]', 'DCache1', 'xtintfDataCache', 'TagCData', '0.09', '-' ],
    'DTagDData1'         => [ 'input',   '[22:0]', 'DCache1', 'xtintfDataCache', 'TagDData', '0.09', '-' ],
    'DTagAData0'         => [ 'input',   '[22:0]', 'DCache0', 'xtintfDataCache', 'TagAData', '0.09', '-' ],
    'DTagBData0'         => [ 'input',   '[22:0]', 'DCache0', 'xtintfDataCache', 'TagBData', '0.09', '-' ],
    'DTagCData0'         => [ 'input',   '[22:0]', 'DCache0', 'xtintfDataCache', 'TagCData', '0.09', '-' ],
    'DTagDData0'         => [ 'input',   '[22:0]', 'DCache0', 'xtintfDataCache', 'TagDData', '0.09', '-' ],
    'DCacheADataB0'      => [ 'input',   '[63:0]', 'DCache', '', 'CacheADataB0', '0.09', '-' ],
    'DCacheBDataB0'      => [ 'input',   '[63:0]', 'DCache', '', 'CacheBDataB0', '0.09', '-' ],
    'DCacheCDataB0'      => [ 'input',   '[63:0]', 'DCache', '', 'CacheCDataB0', '0.09', '-' ],
    'DCacheDDataB0'      => [ 'input',   '[63:0]', 'DCache', '', 'CacheDDataB0', '0.09', '-' ],
    'DCacheADataB1'      => [ 'input',   '[63:0]', 'DCache', '', 'CacheADataB1', '0.09', '-' ],
    'DCacheBDataB1'      => [ 'input',   '[63:0]', 'DCache', '', 'CacheBDataB1', '0.09', '-' ],
    'DCacheCDataB1'      => [ 'input',   '[63:0]', 'DCache', '', 'CacheCDataB1', '0.09', '-' ],
    'DCacheDDataB1'      => [ 'input',   '[63:0]', 'DCache', '', 'CacheDDataB1', '0.09', '-' ],
    'DRam0BusyB0'        => [ 'input',         '', 'DRam0B0', 'xtintfDataRam', 'Busy', '0.09', '-' ],
    'DRam0BusyB1'        => [ 'input',         '', 'DRam0B1', 'xtintfDataRam', 'Busy', '0.09', '-' ],
    'DRam0DataB0'        => [ 'input',   '[63:0]', 'DRam0B0', 'xtintfDataRam', 'Data', '0.09', '-' ],
    'DRam0DataB1'        => [ 'input',   '[63:0]', 'DRam0B1', 'xtintfDataRam', 'Data', '0.09', '-' ],
    'DRam1BusyB0'        => [ 'input',         '', 'DRam1B0', 'xtintfDataRam', 'Busy', '0.09', '-' ],
    'DRam1BusyB1'        => [ 'input',         '', 'DRam1B1', 'xtintfDataRam', 'Busy', '0.09', '-' ],
    'DRam1DataB0'        => [ 'input',   '[63:0]', 'DRam1B0', 'xtintfDataRam', 'Data', '0.09', '-' ],
    'DRam1DataB1'        => [ 'input',   '[63:0]', 'DRam1B1', 'xtintfDataRam', 'Data', '0.09', '-' ],
    'PIReqRdy'           => [ 'input',         '', 'PIFM', 'xtintfPIF', 'ReqRdy', 'C*0.60', '-' ],
    'PIRespData'         => [ 'input',   '[63:0]', 'PIFM', 'xtintfPIF', 'RespData', 'C*0.70', '-' ],
    'PIRespCntl'         => [ 'input',    '[7:0]', 'PIFM', 'xtintfPIF', 'RespCntl', 'C*0.70', '-' ],
    'PIRespValid'        => [ 'input',         '', 'PIFM', 'xtintfPIF', 'RespValid', 'C*0.70', '-' ],
    'PIRespPriority'     => [ 'input',    '[1:0]', 'PIFM', 'xtintfPIF', 'RespPriority', 'C*0.70', '-' ],
    'PIRespId'           => [ 'input',    '[5:0]', 'PIFM', 'xtintfPIF', 'RespId', 'C*0.70', '-' ],
    'PIReqValid'         => [ 'input',         '', 'PIFS', 'xtintfPIF', 'ReqValid', 'C*0.70', '-' ],
    'PIReqCntl'          => [ 'input',    '[7:0]', 'PIFS', 'xtintfPIF', 'ReqCntl', 'C*0.70', '-' ],
    'PIReqAttribute'     => [ 'input',   '[11:0]', 'PIFS', 'xtintfPIF', 'ReqAttribute', 'C*0.70', '-' ],
    'PIReqAdrs'          => [ 'input',   '[31:0]', 'PIFS', 'xtintfPIF', 'ReqAdrs', 'C*0.70', '-' ],
    'PIReqData'          => [ 'input',   '[63:0]', 'PIFS', 'xtintfPIF', 'ReqData', 'C*0.70', '-' ],
    'PIReqDataBE'        => [ 'input',    '[7:0]', 'PIFS', 'xtintfPIF', 'ReqDataBE', 'C*0.70', '-' ],
    'PIReqPriority'      => [ 'input',    '[1:0]', 'PIFS', 'xtintfPIF', 'ReqPriority', 'C*0.70', '-' ],
    'PIReqId'            => [ 'input',    '[5:0]', 'PIFS', 'xtintfPIF', 'ReqId', 'C*0.70', '-' ],
    'AXIExclID'          => [ 'input',   '[15:0]', 'PIFS', 'xtintfPIF', 'AXIExclID', 'C*0.70', '-' ],
    'PIRespRdy'          => [ 'input',         '', 'PIFS', 'xtintfPIF', 'RespRdy', 'C*0.70', '-' ],
    'PrefetchRamData'    => [ 'input',  '[127:0]', 'Prefetch', 'xtintPrefetch', 'Data', '0.4356', '-' ],
    'BInterrupt'         => [ 'input',   '[17:0]', 'Interrupt', 'xtintfWire', 'Data', 'C*0.40', '-' ],
    'PRID'               => [ 'input',   '[15:0]', 'PRID', 'xtintfWire', 'Data', 'C*0.25', '-' ],
    'StatVectorSel'      => [ 'input',         '', 'Unclassified', '', '', 'C*0.25', '-' ],
    'AltResetVec'        => [ 'input',   '[31:0]', 'AltResetVec', 'xtintfWire', 'Data', 'C*0.25', '-' ],
    'PDebugEnable'       => [ 'input',         '', 'TracePort', 'xtintfTracePort', 'DebugEnable', 'C*0.80', 'D0' ],
    'PerfMonInt'         => [ 'input',         '', 'PerfMon', 'xtintfPerfMon', 'PerfMonInt', 'C*0.80', '-' ],
    'IRam0Busy'          => [ 'input',         '', 'IRam0', 'xtintfInstRam', 'Busy', '0.09', '-' ],
    'IRam1Busy'          => [ 'input',         '', 'IRam1', 'xtintfInstRam', 'Busy', '0.09', '-' ],
    'RunStall'           => [ 'input',         '', 'RunStall', 'xtintfWire', 'Data', 'C*0.80', '-' ],
    'TMode'              => [ 'input',         '', 'TMode', 'xtintfWire', 'Data', 'C*0.10', '-' ],
    'TModeClkGateOverride' => [ 'input',         '', 'TMode', 'xtintfWire', 'Data', 'C*0.10', '-' ],
    'OCDRSRBus_M'        => [ 'input',   '[31:0]', 'OCD', 'xtintfOCD', 'OCDRSRBus_M', 'C*0.7', 'D0' ],
    'OCDEnabled_W'       => [ 'input',         '', 'OCD', 'xtintfOCD', 'OCDEnabled_W', 'C*0.7', 'D0' ],
    'OCDOverride'        => [ 'input',         '', 'OCD', 'xtintfOCD', 'OCDOverride', 'C*0.7', 'D0' ],
    'InterruptAllConds'  => [ 'input',         '', 'OCD', 'xtintfOCD', 'InterruptAllConds', 'C*0.7', 'D0' ],
    'OCDDbgInt'          => [ 'input',         '', 'OCD', 'xtintfOCD', 'OCDDbgInt', 'C*0.7', 'D0' ],
    'ExecDebugInstr'     => [ 'input',         '', 'OCD', 'xtintfOCD', 'ExecDebugInstr', 'C*0.7', 'D0' ],
    'DebugInstr'         => [ 'input',   '[87:0]', 'OCD', 'xtintfOCD', 'DebugInstr', 'C*0.7', 'D0' ],
    'EnableRunStall'     => [ 'input',         '', 'OCD', 'xtintfOCD', 'EnableRunStall', 'C*0.7', 'D1' ],
    'DebugMode'          => [ 'input',         '', 'OCD', 'xtintfOCD', 'DebugMode', 'C*0.7', 'D0' ],
    'TIE_ERI_RD_In'      => [ 'input',   '[31:0]', 'ERI_RD', 'xtintfLookup', 'ERI_RD__DataIn', 'C*0.3', '-' ],
    'TIE_ERI_RD_Rdy'     => [ 'input',         '', 'ERI_RD', 'xtintfLookup', 'ERI_RD__Rdy', 'C*0.5', '-' ],
    'TIE_ERI_WR_In'      => [ 'input',         '', 'ERI_WR', 'xtintfLookup', 'ERI_WR__DataIn', 'C*0.3', '-' ],
    'TIE_IMPWIRE'        => [ 'input',   '[31:0]', 'IMPWIRE', 'xtintfWire', 'IMPWIRE__Data', 'C*0.7', '-' ],
    'TIE_MFP1_In'        => [ 'input',   '[31:0]', 'MFP1', 'xtintfLookup', 'MFP1__DataIn', 'C*0.3', '-' ],
    'TIE_MFP2_In'        => [ 'input',   '[31:0]', 'MFP2', 'xtintfLookup', 'MFP2__DataIn', 'C*0.3', '-' ],
    'TIE_mfp1status'     => [ 'input',    '[7:0]', 'mfp1status', 'xtintfWire', 'mfp1status__Data', 'C*0.7', '-' ],
    'TIE_mfp2status'     => [ 'input',    '[7:0]', 'mfp2status', 'xtintfWire', 'mfp2status__Data', 'C*0.7', '-' ],
    'ICacheAEn'          => [ 'output',         '', 'ICache', 'xtintfInstCache', 'CacheAEn', '0.0968', 'P0' ],
    'ICacheAWordEn'      => [ 'output',    '[3:0]', 'ICache', 'xtintfInstCache', 'CacheAWordEn', '0.0517', 'P0' ],
    'ICacheAWr'          => [ 'output',         '', 'ICache', 'xtintfInstCache', 'CacheAWr', '0.0616', 'P0' ],
    'ITagAEn'            => [ 'output',         '', 'ICache', 'xtintfInstCache', 'TagAEn', '0.0968', 'P0' ],
    'ITagAWr'            => [ 'output',         '', 'ICache', 'xtintfInstCache', 'TagAWr', '0.0539', 'P0' ],
    'ICacheBEn'          => [ 'output',         '', 'ICache', 'xtintfInstCache', 'CacheBEn', '0.0968', 'P0' ],
    'ICacheBWordEn'      => [ 'output',    '[3:0]', 'ICache', 'xtintfInstCache', 'CacheBWordEn', '0.0517', 'P0' ],
    'ICacheBWr'          => [ 'output',         '', 'ICache', 'xtintfInstCache', 'CacheBWr', '0.0616', 'P0' ],
    'ITagBEn'            => [ 'output',         '', 'ICache', 'xtintfInstCache', 'TagBEn', '0.0968', 'P0' ],
    'ITagBWr'            => [ 'output',         '', 'ICache', 'xtintfInstCache', 'TagBWr', '0.0539', 'P0' ],
    'ICacheCEn'          => [ 'output',         '', 'ICache', 'xtintfInstCache', 'CacheCEn', '0.0968', 'P0' ],
    'ICacheCWordEn'      => [ 'output',    '[3:0]', 'ICache', 'xtintfInstCache', 'CacheCWordEn', '0.0517', 'P0' ],
    'ICacheCWr'          => [ 'output',         '', 'ICache', 'xtintfInstCache', 'CacheCWr', '0.0616', 'P0' ],
    'ITagCEn'            => [ 'output',         '', 'ICache', 'xtintfInstCache', 'TagCEn', '0.0968', 'P0' ],
    'ITagCWr'            => [ 'output',         '', 'ICache', 'xtintfInstCache', 'TagCWr', '0.0539', 'P0' ],
    'ICacheDEn'          => [ 'output',         '', 'ICache', 'xtintfInstCache', 'CacheDEn', '0.0968', 'P0' ],
    'ICacheDWordEn'      => [ 'output',    '[3:0]', 'ICache', 'xtintfInstCache', 'CacheDWordEn', '0.0517', 'P0' ],
    'ICacheDWr'          => [ 'output',         '', 'ICache', 'xtintfInstCache', 'CacheDWr', '0.0616', 'P0' ],
    'ITagDEn'            => [ 'output',         '', 'ICache', 'xtintfInstCache', 'TagDEn', '0.0968', 'P0' ],
    'ITagDWr'            => [ 'output',         '', 'ICache', 'xtintfInstCache', 'TagDWr', '0.0539', 'P0' ],
    'ICacheWrData'       => [ 'output',  '[127:0]', 'ICache', 'xtintfInstCache', 'CacheWrData', '0.0517', 'P0' ],
    'ITagWrData'         => [ 'output',   '[21:0]', 'ICache', 'xtintfInstCache', 'TagWrData', '0.0693', 'P0' ],
    'ITagAddr'           => [ 'output',    '[5:0]', 'ICache', 'xtintfInstCache', 'TagAddr', '0.0495', 'P0' ],
    'ICacheAddr'         => [ 'output',    '[8:0]', 'ICache', 'xtintfInstCache', 'CacheAddr', '0.0583', 'P0' ],
    'IRam0Addr'          => [ 'output',   '[15:4]', 'IRam0', 'xtintfInstRam', 'Addr', '0.154', 'P0' ],
    'IRam0En'            => [ 'output',         '', 'IRam0', 'xtintfInstRam', 'En', '0.0968', 'P0' ],
    'IRam0Wr'            => [ 'output',         '', 'IRam0', 'xtintfInstRam', 'Wr', '0.1133', 'P0' ],
    'IRam0WordEn'        => [ 'output',    '[3:0]', 'IRam0', 'xtintfInstRam', 'WordEn', '0.0814', 'P0' ],
    'IRam0WrData'        => [ 'output',  '[127:0]', 'IRam0', 'xtintfInstRam', 'WrData', '0.0814', 'P0' ],
    'IRam0LoadStore'     => [ 'output',         '', 'IRam0', 'xtintfInstRam', 'LoadStore', 'C*0.30', 'XP0' ],
    'IRam1Addr'          => [ 'output',   '[15:4]', 'IRam1', 'xtintfInstRam', 'Addr', '0.154', 'P0' ],
    'IRam1En'            => [ 'output',         '', 'IRam1', 'xtintfInstRam', 'En', '0.0968', 'P0' ],
    'IRam1Wr'            => [ 'output',         '', 'IRam1', 'xtintfInstRam', 'Wr', '0.1133', 'P0' ],
    'IRam1WordEn'        => [ 'output',    '[3:0]', 'IRam1', 'xtintfInstRam', 'WordEn', '0.0814', 'P0' ],
    'IRam1WrData'        => [ 'output',  '[127:0]', 'IRam1', 'xtintfInstRam', 'WrData', '0.0814', 'P0' ],
    'IRam1LoadStore'     => [ 'output',         '', 'IRam1', 'xtintfInstRam', 'LoadStore', 'C*0.30', 'XP0' ],
    'DTagAddr1'          => [ 'output',   '[12:7]', 'DCache1', 'xtintfDataCache', 'TagAddr', '0.0495', 'P0' ],
    'DTagWrData1'        => [ 'output',   '[22:0]', 'DCache1', 'xtintfDataCache', 'TagWrData', '0.0693', 'P0' ],
    'DTagAEn1'           => [ 'output',         '', 'DCache1', 'xtintfDataCache', 'TagAEn', '0.0968', 'P0' ],
    'DTagAWr1'           => [ 'output',         '', 'DCache1', 'xtintfDataCache', 'TagAWr', '0.0539', 'P0' ],
    'DTagBEn1'           => [ 'output',         '', 'DCache1', 'xtintfDataCache', 'TagBEn', '0.0968', 'P0' ],
    'DTagBWr1'           => [ 'output',         '', 'DCache1', 'xtintfDataCache', 'TagBWr', '0.0539', 'P0' ],
    'DTagCEn1'           => [ 'output',         '', 'DCache1', 'xtintfDataCache', 'TagCEn', '0.0968', 'P0' ],
    'DTagCWr1'           => [ 'output',         '', 'DCache1', 'xtintfDataCache', 'TagCWr', '0.0539', 'P0' ],
    'DTagDEn1'           => [ 'output',         '', 'DCache1', 'xtintfDataCache', 'TagDEn', '0.0968', 'P0' ],
    'DTagDWr1'           => [ 'output',         '', 'DCache1', 'xtintfDataCache', 'TagDWr', '0.0539', 'P0' ],
    'DCacheAddrB0'       => [ 'output',   '[12:4]', 'DCache', '', 'CacheAddrB0', '0.0561', 'P0' ],
    'DCacheAddrB1'       => [ 'output',   '[12:4]', 'DCache', '', 'CacheAddrB1', '0.0561', 'P0' ],
    'DTagAddr0'          => [ 'output',   '[12:7]', 'DCache0', 'xtintfDataCache', 'TagAddr', '0.0495', 'P0' ],
    'DTagWrData0'        => [ 'output',   '[22:0]', 'DCache0', 'xtintfDataCache', 'TagWrData', '0.0693', 'P0' ],
    'DTagAEn0'           => [ 'output',         '', 'DCache0', 'xtintfDataCache', 'TagAEn', '0.0968', 'P0' ],
    'DTagAWr0'           => [ 'output',         '', 'DCache0', 'xtintfDataCache', 'TagAWr', '0.0539', 'P0' ],
    'DTagBEn0'           => [ 'output',         '', 'DCache0', 'xtintfDataCache', 'TagBEn', '0.0968', 'P0' ],
    'DTagBWr0'           => [ 'output',         '', 'DCache0', 'xtintfDataCache', 'TagBWr', '0.0539', 'P0' ],
    'DTagCEn0'           => [ 'output',         '', 'DCache0', 'xtintfDataCache', 'TagCEn', '0.0968', 'P0' ],
    'DTagCWr0'           => [ 'output',         '', 'DCache0', 'xtintfDataCache', 'TagCWr', '0.0539', 'P0' ],
    'DTagDEn0'           => [ 'output',         '', 'DCache0', 'xtintfDataCache', 'TagDEn', '0.0968', 'P0' ],
    'DTagDWr0'           => [ 'output',         '', 'DCache0', 'xtintfDataCache', 'TagDWr', '0.0539', 'P0' ],
    'DCacheWrDataB0'     => [ 'output',   '[63:0]', 'DCache', '', 'CacheWrDataB0', '0.0594', 'P0' ],
    'DCacheByteEnB0'     => [ 'output',    '[7:0]', 'DCache', '', 'CacheByteEnB0', '0.0594', 'P0' ],
    'DCacheAEnB0'        => [ 'output',         '', 'DCache', '', 'CacheAEnB0', '0.0968', 'P0' ],
    'DCacheAWrB0'        => [ 'output',         '', 'DCache', '', 'CacheAWrB0', '0.0572', 'P0' ],
    'DCacheBEnB0'        => [ 'output',         '', 'DCache', '', 'CacheBEnB0', '0.0968', 'P0' ],
    'DCacheBWrB0'        => [ 'output',         '', 'DCache', '', 'CacheBWrB0', '0.0572', 'P0' ],
    'DCacheCEnB0'        => [ 'output',         '', 'DCache', '', 'CacheCEnB0', '0.0968', 'P0' ],
    'DCacheCWrB0'        => [ 'output',         '', 'DCache', '', 'CacheCWrB0', '0.0572', 'P0' ],
    'DCacheDEnB0'        => [ 'output',         '', 'DCache', '', 'CacheDEnB0', '0.0968', 'P0' ],
    'DCacheDWrB0'        => [ 'output',         '', 'DCache', '', 'CacheDWrB0', '0.0572', 'P0' ],
    'DCacheWrDataB1'     => [ 'output',   '[63:0]', 'DCache', '', 'CacheWrDataB1', '0.0594', 'P0' ],
    'DCacheByteEnB1'     => [ 'output',    '[7:0]', 'DCache', '', 'CacheByteEnB1', '0.0594', 'P0' ],
    'DCacheAEnB1'        => [ 'output',         '', 'DCache', '', 'CacheAEnB1', '0.0968', 'P0' ],
    'DCacheAWrB1'        => [ 'output',         '', 'DCache', '', 'CacheAWrB1', '0.0572', 'P0' ],
    'DCacheBEnB1'        => [ 'output',         '', 'DCache', '', 'CacheBEnB1', '0.0968', 'P0' ],
    'DCacheBWrB1'        => [ 'output',         '', 'DCache', '', 'CacheBWrB1', '0.0572', 'P0' ],
    'DCacheCEnB1'        => [ 'output',         '', 'DCache', '', 'CacheCEnB1', '0.0968', 'P0' ],
    'DCacheCWrB1'        => [ 'output',         '', 'DCache', '', 'CacheCWrB1', '0.0572', 'P0' ],
    'DCacheDEnB1'        => [ 'output',         '', 'DCache', '', 'CacheDEnB1', '0.0968', 'P0' ],
    'DCacheDWrB1'        => [ 'output',         '', 'DCache', '', 'CacheDWrB1', '0.0572', 'P0' ],
    'DRam0AddrB0'        => [ 'output',   '[17:4]', 'DRam0B0', 'xtintfDataRam', 'Addr', '0.154', 'P0' ],
    'DRam0EnB0'          => [ 'output',         '', 'DRam0B0', 'xtintfDataRam', 'En', '0.0968', 'P0' ],
    'DRam0ByteEnB0'      => [ 'output',    '[7:0]', 'DRam0B0', 'xtintfDataRam', 'ByteEn', '0.1089', 'P0' ],
    'DRam0WrB0'          => [ 'output',         '', 'DRam0B0', 'xtintfDataRam', 'Wr', '0.0935', 'P0' ],
    'DRam0WrDataB0'      => [ 'output',   '[63:0]', 'DRam0B0', 'xtintfDataRam', 'WrData', '0.1089', 'P0' ],
    'DRam0AddrB1'        => [ 'output',   '[17:4]', 'DRam0B1', 'xtintfDataRam', 'Addr', '0.154', 'P0' ],
    'DRam0EnB1'          => [ 'output',         '', 'DRam0B1', 'xtintfDataRam', 'En', '0.0968', 'P0' ],
    'DRam0ByteEnB1'      => [ 'output',    '[7:0]', 'DRam0B1', 'xtintfDataRam', 'ByteEn', '0.1089', 'P0' ],
    'DRam0WrB1'          => [ 'output',         '', 'DRam0B1', 'xtintfDataRam', 'Wr', '0.0935', 'P0' ],
    'DRam0WrDataB1'      => [ 'output',   '[63:0]', 'DRam0B1', 'xtintfDataRam', 'WrData', '0.1089', 'P0' ],
    'DRam1AddrB0'        => [ 'output',   '[17:4]', 'DRam1B0', 'xtintfDataRam', 'Addr', '0.154', 'P0' ],
    'DRam1EnB0'          => [ 'output',         '', 'DRam1B0', 'xtintfDataRam', 'En', '0.0968', 'P0' ],
    'DRam1ByteEnB0'      => [ 'output',    '[7:0]', 'DRam1B0', 'xtintfDataRam', 'ByteEn', '0.1089', 'P0' ],
    'DRam1WrB0'          => [ 'output',         '', 'DRam1B0', 'xtintfDataRam', 'Wr', '0.0935', 'P0' ],
    'DRam1WrDataB0'      => [ 'output',   '[63:0]', 'DRam1B0', 'xtintfDataRam', 'WrData', '0.1089', 'P0' ],
    'DRam1AddrB1'        => [ 'output',   '[17:4]', 'DRam1B1', 'xtintfDataRam', 'Addr', '0.154', 'P0' ],
    'DRam1EnB1'          => [ 'output',         '', 'DRam1B1', 'xtintfDataRam', 'En', '0.0968', 'P0' ],
    'DRam1ByteEnB1'      => [ 'output',    '[7:0]', 'DRam1B1', 'xtintfDataRam', 'ByteEn', '0.1089', 'P0' ],
    'DRam1WrB1'          => [ 'output',         '', 'DRam1B1', 'xtintfDataRam', 'Wr', '0.0935', 'P0' ],
    'DRam1WrDataB1'      => [ 'output',   '[63:0]', 'DRam1B1', 'xtintfDataRam', 'WrData', '0.1089', 'P0' ],
    'POReqValid'         => [ 'output',         '', 'PIFM', 'xtintfPIF', 'ReqValid', 'C*0.80', '-' ],
    'POReqCntl'          => [ 'output',    '[7:0]', 'PIFM', 'xtintfPIF', 'ReqCntl', 'C*0.80', '-' ],
    'POReqAttribute'     => [ 'output',   '[11:0]', 'PIFM', 'xtintfPIF', 'ReqAttribute', 'C*0.80', '-' ],
    'POReqAdrs'          => [ 'output',   '[31:0]', 'PIFM', 'xtintfPIF', 'ReqAdrs', 'C*0.80', '-' ],
    'POReqData'          => [ 'output',   '[63:0]', 'PIFM', 'xtintfPIF', 'ReqData', 'C*0.80', '-' ],
    'POReqDataBE'        => [ 'output',    '[7:0]', 'PIFM', 'xtintfPIF', 'ReqDataBE', 'C*0.80', '-' ],
    'POReqPriority'      => [ 'output',    '[1:0]', 'PIFM', 'xtintfPIF', 'ReqPriority', 'C*0.80', '-' ],
    'POReqId'            => [ 'output',    '[5:0]', 'PIFM', 'xtintfPIF', 'ReqId', 'C*0.80', '-' ],
    'PORespRdy'          => [ 'output',         '', 'PIFM', 'xtintfPIF', 'RespRdy', 'C*0.50', '-' ],
    'POReqRdy'           => [ 'output',         '', 'PIFS', 'xtintfPIF', 'ReqRdy', 'C*0.80', '-' ],
    'PORespData'         => [ 'output',   '[63:0]', 'PIFS', 'xtintfPIF', 'RespData', 'C*0.80', '-' ],
    'PORespCntl'         => [ 'output',    '[7:0]', 'PIFS', 'xtintfPIF', 'RespCntl', 'C*0.80', '-' ],
    'PORespValid'        => [ 'output',         '', 'PIFS', 'xtintfPIF', 'RespValid', 'C*0.80', '-' ],
    'PORespPriority'     => [ 'output',    '[1:0]', 'PIFS', 'xtintfPIF', 'RespPriority', 'C*0.80', '-' ],
    'PORespId'           => [ 'output',    '[5:0]', 'PIFS', 'xtintfPIF', 'RespId', 'C*0.80', '-' ],
    'PrefetchRamEn'      => [ 'output',         '', 'Prefetch', 'xtintPrefetch', 'En', '0.0968', '-' ],
    'PrefetchRamPIFWEn'  => [ 'output',    '[1:0]', 'Prefetch', 'xtintPrefetch', 'PIFWEn', '0.0968', '-' ],
    'PrefetchRamWr'      => [ 'output',         '', 'Prefetch', 'xtintPrefetch', 'Wr', '0.0539', '-' ],
    'PrefetchRamAddr'    => [ 'output',    '[6:0]', 'Prefetch', 'xtintPrefetch', 'Addr', '0.0517', '-' ],
    'PrefetchRamWrData'  => [ 'output',  '[127:0]', 'Prefetch', 'xtintPrefetch', 'WrData', '0.0605', '-' ],
    'PDebugLS0Stat'      => [ 'output',   '[31:0]', 'TracePort', 'xtintfTracePort', 'DebugLS', 'C*0.20', 'P0' ],
    'PDebugLS0Addr'      => [ 'output',   '[31:0]', 'TracePort', 'xtintfTracePort', 'DebugLS', 'C*0.20', 'P0' ],
    'PDebugLS0Data'      => [ 'output',   '[31:0]', 'TracePort', 'xtintfTracePort', 'DebugLS', 'C*0.20', 'P0' ],
    'PDebugLS1Stat'      => [ 'output',   '[31:0]', 'TracePort', 'xtintfTracePort', 'DebugLS', 'C*0.20', 'P0' ],
    'PDebugLS1Addr'      => [ 'output',   '[31:0]', 'TracePort', 'xtintfTracePort', 'DebugLS', 'C*0.20', 'P0' ],
    'PDebugLS1Data'      => [ 'output',   '[31:0]', 'TracePort', 'xtintfTracePort', 'DebugLS', 'C*0.20', 'P0' ],
    'PDebugOutPif'       => [ 'output',    '[7:0]', 'TracePort', 'xtintfTracePort', 'DebugPif', 'C*0.20', 'P0' ],
    'PDebugInbPif'       => [ 'output',    '[7:0]', 'TracePort', 'xtintfTracePort', 'DebugPif', 'C*0.20', 'P0' ],
    'PDebugPrefetchLookup' => [ 'output',    '[7:0]', 'TracePort', 'xtintfTracePort', 'DebugPif', 'C*0.20', 'P0' ],
    'PDebugPrefetchL1Fill' => [ 'output',    '[3:0]', 'TracePort', 'xtintfTracePort', 'DebugPif', 'C*0.20', 'P0' ],
    'PDebugData'         => [ 'output',   '[31:0]', 'TracePort', 'xtintfTracePort', 'DebugData', 'C*0.20', 'P0' ],
    'PDebugInst'         => [ 'output',   '[31:0]', 'TracePort', 'xtintfTracePort', 'DebugInst', 'C*0.20', 'P0' ],
    'PDebugStatus'       => [ 'output',    '[7:0]', 'TracePort', 'xtintfTracePort', 'DebugStatus', 'C*0.20', 'P0' ],
    'PDebugPC'           => [ 'output',   '[31:0]', 'TracePort', 'xtintfTracePort', 'DebugPC', 'C*0.20', 'P0' ],
    'PWaitMode'          => [ 'output',         '', 'Interrupt', '', 'WaitMode', 'C*0.85', 'XP1' ],
    'GlobalStall'        => [ 'output',         '', 'OCD', 'xtintfOCD', 'GlobalStall', 'C*0.7', 'P0' ],
    'Valid_W'            => [ 'output',         '', 'OCD', 'xtintfOCD', 'Valid_W', 'C*0.7', 'P0' ],
    'RealExcept_W'       => [ 'output',         '', 'OCD', 'xtintfOCD', 'RealExcept_W', 'C*0.7', 'P0' ],
    'RFDO_W'             => [ 'output',         '', 'OCD', 'xtintfOCD', 'RFDO_W', 'C*0.7', 'P0' ],
    'RFDD_W'             => [ 'output',         '', 'OCD', 'xtintfOCD', 'RFDD_W', 'C*0.7', 'P0' ],
    'ReceivedRunStall'   => [ 'output',         '', 'OCD', 'xtintfOCD', 'ReceivedRunStall', 'C*0.7', 'P0' ],
    'InOCDMode_ps_C4'    => [ 'output',         '', 'OCD', 'xtintfOCD', 'InOCDMode_ps_C4', 'C*0.7', 'P0' ],
    'Interrupt4_1hot_EXC' => [ 'output',         '', 'OCD', 'xtintfOCD', 'Interrupt4_1hot_EXC', 'C*0.7', 'P0' ],
    'PSCurIntLvl_W'      => [ 'output',    '[3:0]', 'OCD', 'xtintfOCD', 'PSCurIntLvl_W', 'C*0.7', 'P0' ],
    'WSRBus_W'           => [ 'output',   '[31:0]', 'OCD', 'xtintfOCD', 'WSRBus_W', 'C*0.7', 'P0' ],
    'SRWrite_W'          => [ 'output',         '', 'OCD', 'xtintfOCD', 'SRWrite_W', 'C*0.7', 'P0' ],
    'SRAddr_W'           => [ 'output',    '[7:0]', 'OCD', 'xtintfOCD', 'SRAddr_W', 'C*0.7', 'P0' ],
    'SRRead_W'           => [ 'output',         '', 'OCD', 'xtintfOCD', 'SRRead_W', 'C*0.7', 'P0' ],
    'TIE_ERI_RD_Out'     => [ 'output',   '[13:0]', 'ERI_RD', 'xtintfLookup', 'ERI_RD__DataOut', 'C*0.3', 'XP0' ],
    'TIE_ERI_RD_Out_Req' => [ 'output',         '', 'ERI_RD', 'xtintfLookup', 'ERI_RD__Req', 'C*0.7', 'XP0' ],
    'TIE_ERI_WR_Out'     => [ 'output',   '[45:0]', 'ERI_WR', 'xtintfLookup', 'ERI_WR__DataOut', 'C*0.3', 'XP0' ],
    'TIE_ERI_WR_Out_Req' => [ 'output',         '', 'ERI_WR', 'xtintfLookup', 'ERI_WR__Req', 'C*0.7', 'XP0' ],
    'TIE_MFP1_Out'       => [ 'output',   '[34:0]', 'MFP1', 'xtintfLookup', 'MFP1__DataOut', 'C*0.3', 'XP0' ],
    'TIE_MFP1_Out_Req'   => [ 'output',         '', 'MFP1', 'xtintfLookup', 'MFP1__Req', 'C*0.7', 'XP0' ],
    'TIE_MFP2_Out'       => [ 'output',   '[38:0]', 'MFP2', 'xtintfLookup', 'MFP2__DataOut', 'C*0.3', 'XP0' ],
    'TIE_MFP2_Out_Req'   => [ 'output',         '', 'MFP2', 'xtintfLookup', 'MFP2__Req', 'C*0.7', 'XP0' ],
    'TIE_EXPSTATE'       => [ 'output',   '[31:0]', 'EXPSTATE', 'xtintfWire', 'EXPSTATE__Data', 'C*0.7', 'XP0' ],
    'TIE_qi'             => [ 'output',    '[4:0]', 'qi', 'xtintfWire', 'qi__Data', 'C*0.7', 'XP0' ],
    'TIE_qo'             => [ 'output',    '[4:0]', 'qo', 'xtintfWire', 'qo__Data', 'C*0.7', 'XP0' ],
    'TIE_qo_trig'        => [ 'output',    '[4:0]', 'qo_trig', 'xtintfWire', 'qo_trig__Data', 'C*0.7', 'XP0' ],
    'EncodedExceptionVector_W' => [ 'output',    '[4:0]', 'FaultInterface', '', 'EncodedExceptionVector_W', 'C*0.2', 'P0' ],
   );

sub getGroups {
    my %groups;
    foreach (keys %ports) {
        next if($ports{$_}->[2] eq 'Global');
        $groups{$ports{$_}->[2]}++;
    }
    sort keys %groups;
}
sub getGroupsSignals {
    my ($group) = @_;
    my %signals;
    foreach (keys %ports) {
        $signals{$_}++ if($ports{$_}->[2] eq $group);
    }
    sort keys %signals;
}
sub getGroupsOutputs {
    my ($group) = @_;
    my %signals;
    foreach (keys %ports) {
        $signals{$_}++ if(($ports{$_}->[2] eq $group) && ($ports{$_}->[0] eq 'output'));
    }
    sort keys %signals;
}
sub getGroupsInputs {
    my ($group) = @_;
    my %signals;
    foreach (keys %ports) {
        $signals{$_}++ if(($ports{$_}->[2] eq $group) && ($ports{$_}->[0] eq 'input'));
    }
    sort keys %signals;
}
sub getSignalsDirection {
    my ($signal) = @_;
    $ports{$signal}->[0];
}
sub getDir {
    getSignalsDirection(@_)
}
sub getSignalsBitIndex {
    my ($signal) = @_;
    $ports{$signal}->[1];
}
sub getSignalsWidth {
    my ($signal) = @_;
    croak "Error: cannot find signal $signal\n" if(!exists $ports{$signal});
    if ($ports{$signal}->[1] eq "") {
        return 1;
    } elsif ($ports{$signal}->[1] =~ /\A \[? (\d+) : (\d+) \]? \z/xms) {
        my ($msb, $lsb) = ($1, $2);
        return($msb - $lsb + 1);
    } else {
	 croak "Error: unrecognized bit index string: $ports{$signal}->[1] \n";
    }
}
sub getSignalsGroup {
    my ($signal) = @_;
    $ports{$signal}->[2];
}
sub getPorts {
    my %remove_dir;
    foreach (keys %ports) {
       $remove_dir{$_} = $ports{$_}->[1];
    }
    \%remove_dir;
}
sub getSignalsInterfaceType {
    my ($signal) = @_;
    $ports{$signal}->[3];
}
sub getSignalsProtocol {
    my ($signal) = @_;
    $ports{$signal}->[4];
}
sub getSignalsDelay {
    my ($signal) = @_;
    $ports{$signal}->[5];
}
sub getSignalsIsoVal {
    my ($signal) = @_;
    $ports{$signal}->[6];
}
sub getIOC {
    my %ConsHash;
    foreach my $group ('Global', getGroups) {
        foreach my $signal (getGroupsSignals($group)) {
            next if($signal =~ /CLK$/);
            next if($signal eq 'JTCK');
            next if($signal eq 'JTDI');
            (my $dir = getDir($signal)) =~ s/^([io]).+$/$1/;
            (my $timing = getSignalsDelay($signal)) =~ s/C\*/Target_ClockPeriod \* /;
            my $clock = ($group =~ /^PIF[MS]$/) ? 'BCLK' :
                        ($group eq 'TAP')       ? 'JTCK' :
                        ($group eq 'APB')       ? 'PBCLK' :
                                                  'CLK';
	    $ConsHash{$group}{$signal} = {dir => $dir, clock => $clock, timing => $timing};
        }
    }
    return %ConsHash;
}
1;

