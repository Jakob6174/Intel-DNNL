/*******************************************************************************
* Copyright 2017-2019 Intel Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#ifndef CPU_BARRIER_HPP
#define CPU_BARRIER_HPP

#include <assert.h>

#include "jit_generator.hpp"
#include "utils.hpp"

namespace dnnl {
namespace impl {
namespace cpu {

namespace simple_barrier {

#ifdef _WIN32
#define CTX_ALIGNMENT 64
#else
#define CTX_ALIGNMENT 4096
#endif

STRUCT_ALIGN(
        CTX_ALIGNMENT, struct ctx_t {
            enum { CACHE_LINE_SIZE = 64 };
            volatile size_t ctr;
            char pad1[CACHE_LINE_SIZE - 1 * sizeof(size_t)];
            volatile size_t sense;
            char pad2[CACHE_LINE_SIZE - 1 * sizeof(size_t)];
        });

inline void ctx_init(ctx_t *ctx) {
    *ctx = utils::zero<ctx_t>();
}
void barrier(ctx_t *ctx, int nthr);

/** injects actual barrier implementation into another jitted code
 * @params:
 *   code      -- jit_generator object where the barrier is to be injected
 *   reg_ctx   -- read-only register with pointer to the barrier context
 *   reg_nnthr -- read-only register with the # of synchronizing threads
 */
void generate(jit_generator &code, Xbyak::Reg64 reg_ctx, Xbyak::Reg64 reg_nthr);

} // namespace simple_barrier

} // namespace cpu
} // namespace impl
} // namespace dnnl

#endif

// vim: et ts=4 sw=4 cindent cino+=l0,\:4,N-s
