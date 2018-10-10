// Copyright 2018 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <ddk/protocol/i2c.h>
#include <ddktl/device.h>
#include <ddktl/mmio.h>
#include <ddktl/protocol/i2c-impl.h>
#include <fbl/array.h>
#include <fbl/atomic.h>
#include <fbl/optional.h>
#include <lib/zx/interrupt.h>
#include <lib/zx/port.h>

#include <threads.h>

namespace mt8167_i2c {

class Mt8167I2c;
using DeviceType = ddk::Device<Mt8167I2c, ddk::Unbindable>;
using MmioBuffer = fbl::optional<ddk::MmioBuffer>;

class Mt8167I2c : public DeviceType,
                     public ddk::I2cImplProtocol<Mt8167I2c> {
public:
    Mt8167I2c(zx_device_t* parent)
        : DeviceType(parent) {}
    zx_status_t Init();

    // Methods required by the ddk mixins
    void DdkUnbind();
    void DdkRelease();
    uint32_t I2cImplGetBusCount();
    zx_status_t I2cImplGetMaxTransferSize(uint32_t bus_id, size_t* out_size);
    zx_status_t I2cImplSetBitRate(uint32_t bus_id, uint32_t bitrate);
    zx_status_t I2cImplTransact(uint32_t bus_id, i2c_impl_op_t* ops, size_t count);

private:
    enum class Wait {
        kBusy,
        kIdle,
        kInterruptPending
    };

    void IrqThread();

    uint32_t bus_count_;
    fbl::Array<MmioBuffer> mmios_;
    MmioBuffer xo_mmio_;

    fbl::Array<zx::interrupt> irqs_;
    zx::port irq_port_;
    thrd_t irq_thread_;

    void Reset();
    zx_status_t Read(uint8_t addr, void* buf, size_t len, bool stop);
    zx_status_t Write(uint8_t addr, const void* buf, size_t len, bool stop);
    zx_status_t Start();
    void Stop();
    zx_status_t RxData(uint8_t* buf, size_t length, bool stop);
    zx_status_t TxData(const uint8_t* buf, size_t length, bool stop);
    zx_status_t TxAddress(uint8_t addr, bool is_read);
    zx_status_t WaitFor(Wait type);
};
} // namespace mt8167_i2c