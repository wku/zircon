// Copyright 2018 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <ddk/platform-defs.h>
#include <ddk/protocol/mipicsi2.h>
#include <ddk/protocol/platform-bus.h>
#include <ddk/protocol/platform-device.h>
#include <ddktl/device.h>
#include <ddktl/mmio.h>
#include <fbl/unique_ptr.h>

namespace camera {

class AmlMipiDevice {
public:
    static zx_status_t Create(zx_device_t* parent);
    DISALLOW_COPY_AND_ASSIGN_ALLOW_MOVE(AmlMipiDevice);
    AmlMipiDevice(){};

    zx_device_t* device_;

    // Methods required by the ddk.
    // TODO(braval) - Use DDKTL to add child devices when
    //                support is added (ZX-2793)
    void DdkRelease(void* ctx);
    void DdkUnbind(void* ctx);
    void ShutDown();

    // Methods for ZX_PROTOCOL_MIPI_CSI.
    static zx_status_t MipiCsi2Init(void* ctx, const mipi_info_t* info);
    static zx_status_t MipiCsi2DeInit(void* ctx);

private:
    fbl::unique_ptr<ddk::MmioBuffer> csi_phy0_mmio_;
    fbl::unique_ptr<ddk::MmioBuffer> aphy0_mmio_;
    fbl::unique_ptr<ddk::MmioBuffer> csi_host0_mmio_;
    fbl::unique_ptr<ddk::MmioBuffer> mipi_adap_mmio_;

    platform_device_protocol_t pdev_;

    zx_status_t InitPdev(zx_device_t* parent);
};

} // namespace camera
