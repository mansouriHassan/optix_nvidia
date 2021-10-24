/* 
 * Copyright (c) 2013-2020, NVIDIA CORPORATION. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of NVIDIA CORPORATION nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "app_config.h"

#include <optix.h>

#include "system_parameter.h"
#include "half_common.h"

extern "C" __constant__ SystemParameter sysParameter;

extern "C" __global__ void __exception__all()
{
  // This assumes that the launch dimensions are matching the size of the output buffer.
  const uint3 theLaunchDim   = optixGetLaunchDimensions(); 
  const uint3 theLaunchIndex = optixGetLaunchIndex();

  //const int theExceptionCode = optixGetExceptionCode();
  //printf("Exception %d at (%u, %u)\n", theExceptionCode, theLaunchIndex.x, theLaunchIndex.y);

  const unsigned int index = theLaunchIndex.y * theLaunchDim.x + theLaunchIndex.x;

#if USE_FP32_OUTPUT
  float4* buffer = reinterpret_cast<float4*>(sysParameter.outputBuffer);
  buffer[index] = make_float4(1000000.0f, 0.0f, 1000000.0f, 1.0f); // RGBA32F super magenta
#else
  Half4* buffer = reinterpret_cast<Half4*>(sysParameter.outputBuffer);
  buffer[index] = make_Half4(1000000.0f, 0.0f, 1000000.0f, 1.0f); // RGBA16F super magenta
#endif
}
