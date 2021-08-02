#include "DpPathConnection.h"
#include "DpPathBase.h"
#include "DpWrapper_ISP.h"
#include "mmsys_config.h"

const DpPathConnection::mout_t DpPathConnection::s_moutMap[MOUT_MAP_SIZE] =
{
    { tCAMIN,       { tSCL0,    tSCL1,   tNone,   tNone, tNone },  MMSYS_ISP_MOUT_EN   },
    { tRDMA0,       { tSCL0,    tSCL1,   tNone,   tNone, tNone },  MMSYS_MDP_RDMA0_MOUT_EN  },
    { tSCL0,        { tWDMA,    tWROT0,  tTDSHP0, tSCL1, tNone },  MMSYS_MDP_PRZ0_MOUT_EN  },
    { tSCL1,        { tWDMA,    tWROT0,  tTDSHP0, tNone, tNone },  MMSYS_MDP_PRZ1_MOUT_EN  },
    { tTDSHP0,      { tWDMA,    tWROT0,  tNone,   tNone, tNone },  MMSYS_MDP_TDSHP0_MOUT_EN },
};

const DpPathConnection::sel_t DpPathConnection::s_selInMap[SEL_IN_SIZE] =
{
    { tSCL0,    { tCAMIN,  tRDMA0, tNone,   tNone, tNone, tNone, tNone, tNone }, MMSYS_MDP_PRZ0_SEL_IN },
    { tSCL1,    { tCAMIN,  tRDMA0, tSCL0,   tNone, tNone, tNone, tNone, tNone }, MMSYS_MDP_PRZ1_SEL_IN },
    { tTDSHP0,  { tSCL0,   tSCL1,  tNone,   tNone, tNone, tNone, tNone, tNone }, MMSYS_MDP_TDSHP0_SEL_IN },
    { tWDMA,    { tSCL0,   tSCL1,  tTDSHP0, tNone, tNone, tNone, tNone, tNone }, MMSYS_MDP_WDMA_SEL_IN },
    { tWROT0,   { tSCL0,   tSCL1,  tTDSHP0, tNone, tNone, tNone, tNone, tNone }, MMSYS_MDP_WROT0_SEL_IN },
};

const DpPathConnection::sout_t DpPathConnection::s_selOutMap[SOUT_MAP_SIZE] =
{
    { tNone,    { tNone,  tNone }, 0},
};


DP_STATUS_ENUM DpPathConnection::initTilePath(struct TILE_PARAM_STRUCT *p_tile_param)
{
    DpPathBase::iterator  iterator;
    DpEngineType          curType;
    int32_t               index;

    /* tile core property */
    TILE_REG_MAP_STRUCT *ptr_tile_reg_map = p_tile_param->ptr_tile_reg_map;

    if (false == m_connected)
    {
        if (false == queryMuxInfo())
        {
            return DP_STATUS_INVALID_PATH;
        }
    }

    for (iterator = m_pPath->begin(); iterator != m_pPath->end(); iterator++)
    {
        if (true == iterator->isOutputDisable())
        {
            continue;
        }

        curType = iterator->getEngineType();
        switch (curType)
        {
        //ISP
        case tIMGI:
            static_cast<DpWrapper_ISP*>(&*iterator)->initTilePath(p_tile_param);
            break;
        //MDP
        case tCAMIN:    ptr_tile_reg_map->CAMIN_EN      = 1; break;
        case tRDMA0:    ptr_tile_reg_map->RDMA0_EN      = 1; break;
        case tSCL0:     ptr_tile_reg_map->PRZ0_EN       = 1; break;
        case tSCL1:     ptr_tile_reg_map->PRZ1_EN       = 1; break;
        case tTDSHP0:   ptr_tile_reg_map->TDSHP0_EN     = 1; break;
        case tWROT0:    ptr_tile_reg_map->WROT0_EN      = 1; break;
        case tWDMA:     ptr_tile_reg_map->WDMA_EN       = 1; break;
        default:        break;
        }
    }

    for (index = 0; index < MOUT_NUM; index++)
    {
        switch (s_moutMap[index].id)
        {
        case tCAMIN:    ptr_tile_reg_map->CAMIN_OUT     = m_mOutInfo[index]; break;
        case tRDMA0:    ptr_tile_reg_map->RDMA0_OUT     = m_mOutInfo[index]; break;
        case tSCL0:     ptr_tile_reg_map->PRZ0_OUT      = m_mOutInfo[index]; break;
        case tSCL1:     ptr_tile_reg_map->PRZ1_OUT      = m_mOutInfo[index]; break;
        case tTDSHP0:   ptr_tile_reg_map->TDSHP0_OUT    = m_mOutInfo[index]; break;
        default:        assert(0);
        }
    }

    for (index = 0; index < SEL_IN_NUM; index++)
    {
        switch (s_selInMap[index].id)
        {
        case tSCL0:     ptr_tile_reg_map->PRZ0_SEL      = m_sInInfo[index]; break;
        case tSCL1:     ptr_tile_reg_map->PRZ1_SEL      = m_sInInfo[index]; break;
        case tTDSHP0:   ptr_tile_reg_map->TDSHP0_SEL    = m_sInInfo[index]; break;
        case tWDMA:     ptr_tile_reg_map->WDMA_SEL      = m_sInInfo[index]; break;
        case tWROT0:    ptr_tile_reg_map->WROT0_SEL     = m_sInInfo[index]; break;
        default:        assert(0);
        }
    }

#if 0
    for (index = 0; index < SEL_OUT_NUM; index++)
    {
        switch (s_selOutMap[index].id)
        {
        //case tBLS:      ptr_tile_reg_map->BLS_SLO       = m_sOutInfo[index]; break;
        default:        assert(0);
        }
    }
#endif

    return DP_STATUS_RETURN_SUCCESS;
}
