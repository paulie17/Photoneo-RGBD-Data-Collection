#pragma once
#include <PhoXi.h>
#include <string>

namespace externalCamera {

std::string selectAvailableDevice(pho::api::PhoXiFactory& factory);
pho::api::PPhoXi connectDevice(
        pho::api::PhoXiFactory& factory,
        const std::string& type);
pho::api::PPhoXi selectAndConnectDevice(pho::api::PhoXiFactory& factory);
void disconnectOrLogOut(pho::api::PPhoXi device);
pho::api::PFrame triggerScanAndGetFrame(pho::api::PPhoXi device);

} // namespace externalCamera
