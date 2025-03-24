/****************************************
 * FILE: Engine/Source/HAL/FTaskGraphImpl.cpp
 ****************************************/
#include "FTaskGraphInterface.hpp"

FTaskGraphInterface& FTaskGraphInterface::Get() {
    assert(GImpl && "FTaskGraphInterface not started!");
    return *GImpl;
}
void FTaskGraphInterface::Startup(int WorkerCount) {
    if (GRunning) return;
    GImpl = new FTaskGraphImplementation();
    GImpl->StartupAll(WorkerCount);
    GRunning = true;
}
void FTaskGraphInterface::Shutdown() {
    if (!GRunning) return;
    GImpl->ShutdownAll();
    delete GImpl;
    GImpl    = nullptr;
    GRunning = false;
}
bool FTaskGraphInterface::IsRunning() {
    return GRunning;
}