/*******************************************************************************
* Copyright 2016-2019 Intel Corporation
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

#ifndef CPU_REF_INNER_PRODUCT_HPP
#define CPU_REF_INNER_PRODUCT_HPP

#include <assert.h>

#include "c_types_map.hpp"
#include "type_helpers.hpp"
#include "utils.hpp"

#include "cpu_inner_product_pd.hpp"

namespace dnnl {
namespace impl {
namespace cpu {

template <impl::data_type_t src_type, impl::data_type_t wei_type = src_type,
        impl::data_type_t dst_type = src_type,
        impl::data_type_t acc_type = dst_type>
struct ref_inner_product_fwd_t : public primitive_impl_t {
    struct pd_t : public cpu_inner_product_fwd_pd_t {
        using cpu_inner_product_fwd_pd_t::cpu_inner_product_fwd_pd_t;

        DECLARE_COMMON_PD_T("ref:any", ref_inner_product_fwd_t);

        status_t init() {
            using namespace data_type;

            bool ok = true && is_fwd() && src_md()->data_type == src_type
                    && weights_md()->data_type == wei_type
                    && desc()->accum_data_type == acc_type
                    && dst_md()->data_type == dst_type
                    && IMPLICATION(with_bias(),
                            utils::one_of(
                                    weights_md(1)->data_type, f32, s32, s8, u8))
                    && attr()->output_scales_.has_default_values()
                    && attr()->post_ops_.len_ <= 1
                    && IMPLICATION(attr()->post_ops_.len_ == 1,
                            attr()->post_ops_.entry_[0].is_relu(true, false))
                    && set_default_params() == status::success;
            return ok ? status::success : status::unimplemented;
        }
    };

    ref_inner_product_fwd_t(const pd_t *apd) : primitive_impl_t(apd) {}

    typedef typename prec_traits<src_type>::type src_data_t;
    typedef typename prec_traits<wei_type>::type wei_data_t;
    typedef typename prec_traits<dst_type>::type dst_data_t;
    typedef typename prec_traits<acc_type>::type acc_data_t;

    virtual status_t execute(const exec_ctx_t &ctx) const override {
        execute_forward(ctx);
        return status::success;
    }

private:
    void execute_forward(const exec_ctx_t &ctx) const;
    const pd_t *pd() const { return (const pd_t *)primitive_impl_t::pd(); }
};

template <impl::data_type_t diff_src_type, impl::data_type_t wei_type,
        impl::data_type_t diff_dst_type,
        impl::data_type_t acc_type = diff_src_type>
struct ref_inner_product_bwd_data_t : public primitive_impl_t {
    struct pd_t : public cpu_inner_product_bwd_data_pd_t {
        using cpu_inner_product_bwd_data_pd_t::cpu_inner_product_bwd_data_pd_t;

        DECLARE_COMMON_PD_T("ref:any", ref_inner_product_bwd_data_t);

        status_t init() {
            bool ok = true && desc()->prop_kind == prop_kind::backward_data
                    && diff_src_md()->data_type == diff_src_type
                    && weights_md()->data_type == wei_type
                    && desc()->accum_data_type == acc_type
                    && diff_dst_md()->data_type == diff_dst_type
                    && attr()->has_default_values()
                    && set_default_params() == status::success;
            return ok ? status::success : status::unimplemented;
        }
    };

    ref_inner_product_bwd_data_t(const pd_t *apd) : primitive_impl_t(apd) {}

    typedef typename prec_traits<diff_src_type>::type diff_src_data_t;
    typedef typename prec_traits<wei_type>::type wei_data_t;
    typedef typename prec_traits<diff_dst_type>::type diff_dst_data_t;
    typedef typename prec_traits<acc_type>::type acc_data_t;

    virtual status_t execute(const exec_ctx_t &ctx) const override {
        execute_backward_data(ctx);
        return status::success;
    }

private:
    void execute_backward_data(const exec_ctx_t &ctx) const;
    const pd_t *pd() const { return (const pd_t *)primitive_impl_t::pd(); }
};

template <impl::data_type_t data_type>
struct ref_inner_product_bwd_weights_t : public primitive_impl_t {
    struct pd_t : public cpu_inner_product_bwd_weights_pd_t {
        using cpu_inner_product_bwd_weights_pd_t::
                cpu_inner_product_bwd_weights_pd_t;

        DECLARE_COMMON_PD_T("ref:any", ref_inner_product_bwd_weights_t);

        status_t init() {
            bool ok = true && desc()->prop_kind == prop_kind::backward_weights
                    && utils::everyone_is(data_type, src_md()->data_type,
                            diff_dst_md()->data_type,
                            diff_weights_md()->data_type)
                    && IMPLICATION(with_bias(),
                            data_type == diff_weights_md(1)->data_type)
                    && attr()->has_default_values()
                    && set_default_params() == status::success;
            return ok ? status::success : status::unimplemented;
        }
    };

    ref_inner_product_bwd_weights_t(const pd_t *apd) : primitive_impl_t(apd) {}
    typedef typename prec_traits<data_type>::type data_t;

    virtual status_t execute(const exec_ctx_t &ctx) const override {
        execute_backward_weights(ctx);
        return status::success;
    }

private:
    void execute_backward_weights(const exec_ctx_t &ctx) const;
    const pd_t *pd() const { return (const pd_t *)primitive_impl_t::pd(); }
};

} // namespace cpu
} // namespace impl
} // namespace dnnl

#endif

// vim: et ts=4 sw=4 cindent cino+=l0,\:4,N-s
