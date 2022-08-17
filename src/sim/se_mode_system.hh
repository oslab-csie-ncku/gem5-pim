#ifndef __SIM_SE_MODE_SYSTEM_HH__
#define __SIM_SE_MODE_SYSTEM_HH__

#include <string>
#include <vector>

namespace gem5
{

class BaseCPU;
class System;
class ThreadContext;

namespace semodesystem
{

extern std::string SEModeSystemName;
/* multistack pim */
extern int MemStackNum;
extern std::vector<std::string> SEModeSystemsName;
/**
 * Return true if the object belongs to the specific SE mode system, and vice
 * versa. In addition, when the SE mode system name is not set, it returns
 * false
 */
bool belongSEsys(const System *const _system);
bool belongSEsys(const BaseCPU *const _cpu);
bool belongSEsys(ThreadContext *const _tc);

} //namespace semodesystem
} //namespace gem5

#endif // __SIM_SE_MODE_SYSTEM_HH__
