﻿//
// Copyright © 2017 Arm Ltd. All rights reserved.
// SPDX-License-Identifier: MIT
//
#pragma once

#include "OpenClTimer.hpp"
#include "backends/ArmComputeTensorUtils.hpp"
#include "backends/CpuTensorHandle.hpp"

#include <Half.hpp>

#define ARMNN_SCOPED_PROFILING_EVENT_CL(name) \
    ARMNN_SCOPED_PROFILING_EVENT_WITH_INSTRUMENTS(armnn::Compute::GpuAcc, \
                                                  name, \
                                                  armnn::OpenClTimer(), \
                                                  armnn::WallClockTimer())

namespace armnn
{

template <typename T>
void CopyArmComputeClTensorData(const T* srcData, arm_compute::CLTensor& dstTensor)
{
    {
        ARMNN_SCOPED_PROFILING_EVENT_CL("MapClTensorForWriting");
        dstTensor.map(true);
    }

    {
        ARMNN_SCOPED_PROFILING_EVENT_CL("CopyToClTensor");
        armcomputetensorutils::CopyArmComputeITensorData<T>(srcData, dstTensor);
    }

    dstTensor.unmap();
}

template <typename T>
void InitialiseArmComputeClTensorData(arm_compute::CLTensor& clTensor, const T* data)
{
    armcomputetensorutils::InitialiseArmComputeTensorEmpty(clTensor);
    CopyArmComputeClTensorData<T>(data, clTensor);
}

inline void InitializeArmComputeClTensorDataForFloatTypes(arm_compute::CLTensor& clTensor,
                                                          const ConstCpuTensorHandle *handle)
{
    BOOST_ASSERT(handle);
    switch(handle->GetTensorInfo().GetDataType())
    {
        case DataType::Float16:
            InitialiseArmComputeClTensorData(clTensor, handle->GetConstTensor<armnn::Half>());
            break;
        case DataType::Float32:
            InitialiseArmComputeClTensorData(clTensor, handle->GetConstTensor<float>());
            break;
        default:
            BOOST_ASSERT_MSG(false, "Unexpected floating point type.");
    }
};

} //namespace armnn
