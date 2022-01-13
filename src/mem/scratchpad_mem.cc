#include "mem/scratchpad_mem.hh"

#include "debug/ScratchpadMemory.hh"
#include "mem/cache/cache.hh"
#include "mem/packet_access.hh"

namespace gem5
{

namespace memory
{

ScratchpadMemory::ScratchpadMemory(const ScratchpadMemoryParams &p) :
    SimpleMemory(p),
    support_flush(p.support_flush),
    reg_flush_addr(p.reg_flush_addr),
    reg_flush_size(p.reg_flush_size)
{
    if (support_flush) {
        if (!range.contains(reg_flush_addr))
            fatal("SPM ranges does not contain reg_flush_addr");
        if (!range.contains(reg_flush_size))
            fatal("SPM ranges does not contain reg_flush_size");
    }
}

void
ScratchpadMemory::initSystemDcaches(void)
{
    /**
     * Get all system data caches
     */
    int i = -1;
    Cache *cache = nullptr;

    // L2 cache
    cache = dynamic_cast<Cache *>(SimObject::find("system.l2"));
    if (cache)
        dcaches.push_back(cache);

    // L1 dcache
    while (1) {
        std::string index_str = i == -1 ? "" : std::to_string(i);
        std::string dcache_name = "system.cpu" + index_str + ".dcache";
        cache = dynamic_cast<Cache *>(SimObject::find(dcache_name.c_str()));

        if (cache)
            dcaches.push_back(cache);

        if (i++ >= 0 && !cache)
            break;
    }

    // Show
    DPRINTF(ScratchpadMemory, "list of all the system data caches:\n");
    for (i = 0; i < dcaches.size(); ++i)
        DPRINTF(ScratchpadMemory, "\t%s\n", dcaches[i]->name());
}

void
ScratchpadMemory::init()
{
    SimpleMemory::init();

    if (support_flush)
        initSystemDcaches();

    // Show all memory requesters
    DPRINTF(ScratchpadMemory, "list of all the memory requesters:\n");
    for (int i = 0; i < _system->maxRequestors(); ++i)
        DPRINTF(ScratchpadMemory, "\t%d: %s\n", i,
                _system->getRequestorName(i));
}

const uint32_t *
ScratchpadMemory::readMem_l(const Addr addr) const
{
    if (range.contains(addr))
        return (const uint32_t *)(pmemAddr + (addr - range.start()));

    return NULL;
}

const uint64_t *
ScratchpadMemory::readMem_q(const Addr addr) const
{
    if (range.contains(addr))
        return (const uint64_t *)(pmemAddr + (addr - range.start()));

    return NULL;
}

void
ScratchpadMemory::flushSystemDcaches(const Addr addr,
                                     const uint32_t size) const
{
    assert(size > 0);

    for (int i = 0; i < dcaches.size(); ++i)
        dcaches[i]->flushCacheRange(addr, size);
}

bool
ScratchpadMemory::needFlush(const PacketPtr pkt) const
{
    assert(pkt);

    if (!support_flush)
        return false;

    if (pkt->getAddr() != reg_flush_size || !pkt->isWrite() ||
        pkt->getSize() != sizeof(uint32_t))
        return false;

    const uint32_t flush_size = pkt->getLE<uint32_t>();
    if (!flush_size)
        return false;

    const uint64_t *flush_addr = readMem_q(reg_flush_addr);
    assert(flush_addr);

    flushSystemDcaches(*flush_addr, flush_size);

    return true;
}

Tick
ScratchpadMemory::recvAtomic(PacketPtr pkt)
{
    needFlush(pkt);
    return SimpleMemory::recvAtomic(pkt);
}

Tick
ScratchpadMemory::recvAtomicBackdoor(PacketPtr pkt, MemBackdoorPtr &_backdoor)
{
    needFlush(pkt);
    return SimpleMemory::recvAtomicBackdoor(pkt, _backdoor);
}

void
ScratchpadMemory::recvFunctional(PacketPtr pkt)
{
    needFlush(pkt);
    SimpleMemory::recvFunctional(pkt);
}

bool
ScratchpadMemory::recvTimingReq(PacketPtr pkt)
{
    needFlush(pkt);
    return SimpleMemory::recvTimingReq(pkt);
}

} // namespace memory
} // namespace gem5
