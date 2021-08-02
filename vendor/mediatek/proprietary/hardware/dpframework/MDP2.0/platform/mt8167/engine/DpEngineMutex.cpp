#include "DpEngineMutex.h"
#include "DpEngineType.h"
#include "DpPathBase.h"
#include "DpCommand.h"
#include "mmsys_mutex.h"

DpEngineMutex::DpEngineMutex(DpPathBase *path)
    : m_pPath(path),
      m_mutexID(-1)
{
}


DpEngineMutex::~DpEngineMutex()
{
}


bool DpEngineMutex::require(DpCommand &command)
{
    DpPathBase::iterator iterator;
    DpEngineType         curType;
    uint32_t             mutexMod;
    uint32_t             mutexSOF;

    // Default value
    m_mutexID = -1;
    mutexMod  = 0;
    mutexSOF  = 0;

    for (iterator = m_pPath->begin(); iterator != m_pPath->end(); iterator++)
    {
        if (true == iterator->isOutputDisable())
        {
            continue;
        }

        curType = iterator->getEngineType();
        switch(curType)
        {
        /**********************************************
         * Name             MSB LSB
         * DISP_MUTEX_MOD   26  0
         *
         * Specify which modules are in this mutex.
         * Every bit denote a module. Bit definition:
         *  0 mdp_rdma
         *  1 mdp_rsz0
         *  2 mdp_rsz1
         *  3 mdp_tdshp
         *  4 mdp_wdma
         *  5 mdp_wrot
         **********************************************/
        case tWROT0:
            mutexMod |= 1 << 5;
            break;
        case tWDMA:
            mutexMod |= 1 << 4;
            break;
        case tTDSHP0:
            mutexMod |= 1 << 3;
            break;
        case tSCL1:
            mutexMod |= 1 << 2;
            break;
        case tSCL0:
            mutexMod |= 1 << 1;
            break;
        case tRDMA0:
            mutexMod |= 1 << 0;
            m_mutexID = DISP_MUTEX_MDP_FIRST + 1;
            break;
        case tIMGI:
            m_mutexID = DISP_MUTEX_MDP_FIRST;
            break;
        case tVENC:
            DPLOGI("tVENC trying to get mutex\n");
            break;
        default:
            break;
        }
    }

    if (m_mutexID == -1)
    {
        DPLOGE("no mutex assigned...\n");
        return false;
    }

    // Reset the mutex Holmes Should remove in next release!!!!
    //MM_REG_WRITE(command, MM_MUTEX_RST(m_mutexID), 0x01, 0x00000001);
    //MM_REG_WRITE(command, MM_MUTEX_RST(m_mutexID), 0x00, 0x00000001);

    // Set mutex modules
    MM_REG_WRITE(command, MM_MUTEX_MOD, mutexMod, 0x07FFFFFF);
    MM_REG_WRITE(command, MM_MUTEX_SOF, mutexSOF, 0x00000007);

    return true;
}


bool DpEngineMutex::release(DpCommand &command)
{
    if (-1 != m_mutexID)
    {
        DPLOGI("DpEngineMutex: release mutex token %d\n", m_mutexID);

        // Enable the mutex
        MM_REG_WRITE(command, MM_MUTEX_EN, 0x1, 0x00000001);

        // Wait mutex done and clear the module list
            MM_REG_POLL(command, MM_MUTEX_EN, 0x0, 0x00000001);
        MM_REG_WRITE(command, MM_MUTEX_MOD, 0x0, 0x07FFFFFF);

        DPLOGI("DpEngineMutex: disable mutex %d begin\n", m_mutexID);
    }
    else
    {
        DPLOGE("DpEngineMutex: incorrect mutex id\n");
        assert(0);
    }

    return true;
}
