
#include <arch/ops.h>
#include <tz_private/system.h>

void MTEE_DmaMapMemory(void *cpu_addr, unsigned int size,
                       MTEE_DMA_DIRECTION direction)
{
    switch (direction)
    {
        case MTEE_DMA_TO_DEVICE:
            arch_clean_cache_range((addr_t)cpu_addr, size);
            break;

        case MTEE_DMA_FROM_DEVICE:
            arch_invalidate_cache_range((addr_t)cpu_addr, size);
            break;

        case MTEE_DMA_BIDIRECTION:
            arch_clean_invalidate_cache_range((addr_t)cpu_addr, size);
            break;

        default:
            break;
    }
}


void MTEE_DmaUnmapMemory(void *cpu_addr, unsigned int size,
                         MTEE_DMA_DIRECTION direction)
{
    switch (direction)
    {
        case MTEE_DMA_FROM_DEVICE:
        case MTEE_DMA_BIDIRECTION:
            arch_clean_invalidate_cache_range((addr_t)cpu_addr, size);
            break;

        case MTEE_DMA_TO_DEVICE:
        default:
            break;
    }
}

