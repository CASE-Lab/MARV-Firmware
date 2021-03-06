/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2019 Erik Moqvist
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * This file was generated by cantools version 36.2.0 Thu Jun  3 11:08:39 2021.
 */

#include <string.h>

#include "ct.h"

static inline uint8_t pack_left_shift_u8(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value << shift) & mask);
}

static inline uint8_t pack_left_shift_u16(
    uint16_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value << shift) & mask);
}

static inline uint8_t pack_right_shift_u16(
    uint16_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value >> shift) & mask);
}

static inline uint16_t unpack_left_shift_u16(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint16_t)((uint16_t)(value & mask) << shift);
}

static inline uint8_t unpack_right_shift_u8(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value & mask) >> shift);
}

static inline uint16_t unpack_right_shift_u16(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint16_t)((uint16_t)(value & mask) >> shift);
}

int ct_heartbeat_tcu_pack(
    uint8_t *dst_p,
    const struct ct_heartbeat_tcu_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 3);

    dst_p[0] |= pack_left_shift_u8(src_p->heartbeat_tcu_status, 0u, 0xffu);
    dst_p[1] |= pack_left_shift_u8(src_p->heartbeat_tcu_error_code, 0u, 0xffu);
    dst_p[2] |= pack_left_shift_u8(src_p->heartbeat_tcu_source, 0u, 0xffu);

    return (3);
}

int ct_heartbeat_tcu_unpack(
    struct ct_heartbeat_tcu_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    dst_p->heartbeat_tcu_status = unpack_right_shift_u8(src_p[0], 0u, 0xffu);
    dst_p->heartbeat_tcu_error_code = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    dst_p->heartbeat_tcu_source = unpack_right_shift_u8(src_p[2], 0u, 0xffu);

    return (0);
}

uint8_t ct_heartbeat_tcu_heartbeat_tcu_status_encode(double value)
{
    return (uint8_t)(value);
}

double ct_heartbeat_tcu_heartbeat_tcu_status_decode(uint8_t value)
{
    return ((double)value);
}

bool ct_heartbeat_tcu_heartbeat_tcu_status_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t ct_heartbeat_tcu_heartbeat_tcu_error_code_encode(double value)
{
    return (uint8_t)(value);
}

double ct_heartbeat_tcu_heartbeat_tcu_error_code_decode(uint8_t value)
{
    return ((double)value);
}

bool ct_heartbeat_tcu_heartbeat_tcu_error_code_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t ct_heartbeat_tcu_heartbeat_tcu_source_encode(double value)
{
    return (uint8_t)(value);
}

double ct_heartbeat_tcu_heartbeat_tcu_source_decode(uint8_t value)
{
    return ((double)value);
}

bool ct_heartbeat_tcu_heartbeat_tcu_source_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int ct_log1_tcu_pack(
    uint8_t *dst_p,
    const struct ct_log1_tcu_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_right_shift_u16(src_p->log1_tcu_aps_raw_out, 4u, 0xffu);
    dst_p[1] |= pack_left_shift_u16(src_p->log1_tcu_aps_raw_out, 4u, 0xf0u);
    dst_p[1] |= pack_right_shift_u16(src_p->log1_tcu_rps_raw_out, 8u, 0x0fu);
    dst_p[2] |= pack_left_shift_u16(src_p->log1_tcu_rps_raw_out, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(src_p->log1_tcu_fc_raw_out, 4u, 0xffu);
    dst_p[4] |= pack_left_shift_u16(src_p->log1_tcu_fc_raw_out, 4u, 0xf0u);
    dst_p[4] |= pack_right_shift_u16(src_p->log1_tcu_aps_out, 8u, 0x0fu);
    dst_p[5] |= pack_left_shift_u16(src_p->log1_tcu_aps_out, 0u, 0xffu);
    dst_p[6] |= pack_right_shift_u16(src_p->log1_tcu_rps_out, 4u, 0xffu);
    dst_p[7] |= pack_left_shift_u16(src_p->log1_tcu_rps_out, 4u, 0xf0u);
    dst_p[7] |= pack_left_shift_u8(src_p->log1_tcu_fc_out, 2u, 0x0cu);
    dst_p[7] |= pack_left_shift_u8(src_p->log1_tcu_status, 1u, 0x02u);

    return (8);
}

int ct_log1_tcu_unpack(
    struct ct_log1_tcu_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->log1_tcu_aps_raw_out = unpack_left_shift_u16(src_p[0], 4u, 0xffu);
    dst_p->log1_tcu_aps_raw_out |= unpack_right_shift_u16(src_p[1], 4u, 0xf0u);
    dst_p->log1_tcu_rps_raw_out = unpack_left_shift_u16(src_p[1], 8u, 0x0fu);
    dst_p->log1_tcu_rps_raw_out |= unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    dst_p->log1_tcu_fc_raw_out = unpack_left_shift_u16(src_p[3], 4u, 0xffu);
    dst_p->log1_tcu_fc_raw_out |= unpack_right_shift_u16(src_p[4], 4u, 0xf0u);
    dst_p->log1_tcu_aps_out = unpack_left_shift_u16(src_p[4], 8u, 0x0fu);
    dst_p->log1_tcu_aps_out |= unpack_right_shift_u16(src_p[5], 0u, 0xffu);
    dst_p->log1_tcu_rps_out = unpack_left_shift_u16(src_p[6], 4u, 0xffu);
    dst_p->log1_tcu_rps_out |= unpack_right_shift_u16(src_p[7], 4u, 0xf0u);
    dst_p->log1_tcu_fc_out = unpack_right_shift_u8(src_p[7], 2u, 0x0cu);
    dst_p->log1_tcu_status = unpack_right_shift_u8(src_p[7], 1u, 0x02u);

    return (0);
}

uint16_t ct_log1_tcu_log1_tcu_aps_raw_out_encode(double value)
{
    return (uint16_t)(value / 2.0);
}

double ct_log1_tcu_log1_tcu_aps_raw_out_decode(uint16_t value)
{
    return ((double)value * 2.0);
}

bool ct_log1_tcu_log1_tcu_aps_raw_out_is_in_range(uint16_t value)
{
    return (value <= 4095u);
}

uint16_t ct_log1_tcu_log1_tcu_rps_raw_out_encode(double value)
{
    return (uint16_t)(value / 2.0);
}

double ct_log1_tcu_log1_tcu_rps_raw_out_decode(uint16_t value)
{
    return ((double)value * 2.0);
}

bool ct_log1_tcu_log1_tcu_rps_raw_out_is_in_range(uint16_t value)
{
    return (value <= 4095u);
}

uint16_t ct_log1_tcu_log1_tcu_fc_raw_out_encode(double value)
{
    return (uint16_t)(value / 2.0);
}

double ct_log1_tcu_log1_tcu_fc_raw_out_decode(uint16_t value)
{
    return ((double)value * 2.0);
}

bool ct_log1_tcu_log1_tcu_fc_raw_out_is_in_range(uint16_t value)
{
    return (value <= 4095u);
}

uint16_t ct_log1_tcu_log1_tcu_aps_out_encode(double value)
{
    return (uint16_t)(value / 0.025);
}

double ct_log1_tcu_log1_tcu_aps_out_decode(uint16_t value)
{
    return ((double)value * 0.025);
}

bool ct_log1_tcu_log1_tcu_aps_out_is_in_range(uint16_t value)
{
    return (value <= 4095u);
}

uint16_t ct_log1_tcu_log1_tcu_rps_out_encode(double value)
{
    return (uint16_t)(value / 0.025);
}

double ct_log1_tcu_log1_tcu_rps_out_decode(uint16_t value)
{
    return ((double)value * 0.025);
}

bool ct_log1_tcu_log1_tcu_rps_out_is_in_range(uint16_t value)
{
    return (value <= 4095u);
}

uint8_t ct_log1_tcu_log1_tcu_fc_out_encode(double value)
{
    return (uint8_t)(value);
}

double ct_log1_tcu_log1_tcu_fc_out_decode(uint8_t value)
{
    return ((double)value);
}

bool ct_log1_tcu_log1_tcu_fc_out_is_in_range(uint8_t value)
{
    return (value <= 3u);
}

uint8_t ct_log1_tcu_log1_tcu_status_encode(double value)
{
    return (uint8_t)(value);
}

double ct_log1_tcu_log1_tcu_status_decode(uint8_t value)
{
    return ((double)value);
}

bool ct_log1_tcu_log1_tcu_status_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

int ct_log2_tcu_pack(
    uint8_t *dst_p,
    const struct ct_log2_tcu_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_right_shift_u16(src_p->log2_tcu_aps_raw_in, 4u, 0xffu);
    dst_p[1] |= pack_left_shift_u16(src_p->log2_tcu_aps_raw_in, 4u, 0xf0u);
    dst_p[1] |= pack_right_shift_u16(src_p->log2_tcu_rps_raw_in, 8u, 0x0fu);
    dst_p[2] |= pack_left_shift_u16(src_p->log2_tcu_rps_raw_in, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(src_p->log2_tcu_fc_raw_in, 4u, 0xffu);
    dst_p[4] |= pack_left_shift_u16(src_p->log2_tcu_fc_raw_in, 4u, 0xf0u);
    dst_p[4] |= pack_right_shift_u16(src_p->log2_tcu_aps_in, 8u, 0x0fu);
    dst_p[5] |= pack_left_shift_u16(src_p->log2_tcu_aps_in, 0u, 0xffu);
    dst_p[6] |= pack_right_shift_u16(src_p->log2_tcu_rps_in, 4u, 0xffu);
    dst_p[7] |= pack_left_shift_u16(src_p->log2_tcu_rps_in, 4u, 0xf0u);
    dst_p[7] |= pack_left_shift_u8(src_p->log2_tcu_fc_in, 2u, 0x0cu);
    dst_p[7] |= pack_left_shift_u8(src_p->log2_tcu_status, 1u, 0x02u);

    return (8);
}

int ct_log2_tcu_unpack(
    struct ct_log2_tcu_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->log2_tcu_aps_raw_in = unpack_left_shift_u16(src_p[0], 4u, 0xffu);
    dst_p->log2_tcu_aps_raw_in |= unpack_right_shift_u16(src_p[1], 4u, 0xf0u);
    dst_p->log2_tcu_rps_raw_in = unpack_left_shift_u16(src_p[1], 8u, 0x0fu);
    dst_p->log2_tcu_rps_raw_in |= unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    dst_p->log2_tcu_fc_raw_in = unpack_left_shift_u16(src_p[3], 4u, 0xffu);
    dst_p->log2_tcu_fc_raw_in |= unpack_right_shift_u16(src_p[4], 4u, 0xf0u);
    dst_p->log2_tcu_aps_in = unpack_left_shift_u16(src_p[4], 8u, 0x0fu);
    dst_p->log2_tcu_aps_in |= unpack_right_shift_u16(src_p[5], 0u, 0xffu);
    dst_p->log2_tcu_rps_in = unpack_left_shift_u16(src_p[6], 4u, 0xffu);
    dst_p->log2_tcu_rps_in |= unpack_right_shift_u16(src_p[7], 4u, 0xf0u);
    dst_p->log2_tcu_fc_in = unpack_right_shift_u8(src_p[7], 2u, 0x0cu);
    dst_p->log2_tcu_status = unpack_right_shift_u8(src_p[7], 1u, 0x02u);

    return (0);
}

uint16_t ct_log2_tcu_log2_tcu_aps_raw_in_encode(double value)
{
    return (uint16_t)(value / 2.0);
}

double ct_log2_tcu_log2_tcu_aps_raw_in_decode(uint16_t value)
{
    return ((double)value * 2.0);
}

bool ct_log2_tcu_log2_tcu_aps_raw_in_is_in_range(uint16_t value)
{
    return (value <= 4095u);
}

uint16_t ct_log2_tcu_log2_tcu_rps_raw_in_encode(double value)
{
    return (uint16_t)(value / 2.0);
}

double ct_log2_tcu_log2_tcu_rps_raw_in_decode(uint16_t value)
{
    return ((double)value * 2.0);
}

bool ct_log2_tcu_log2_tcu_rps_raw_in_is_in_range(uint16_t value)
{
    return (value <= 4095u);
}

uint16_t ct_log2_tcu_log2_tcu_fc_raw_in_encode(double value)
{
    return (uint16_t)(value / 2.0);
}

double ct_log2_tcu_log2_tcu_fc_raw_in_decode(uint16_t value)
{
    return ((double)value * 2.0);
}

bool ct_log2_tcu_log2_tcu_fc_raw_in_is_in_range(uint16_t value)
{
    return (value <= 4095u);
}

uint16_t ct_log2_tcu_log2_tcu_aps_in_encode(double value)
{
    return (uint16_t)(value / 0.025);
}

double ct_log2_tcu_log2_tcu_aps_in_decode(uint16_t value)
{
    return ((double)value * 0.025);
}

bool ct_log2_tcu_log2_tcu_aps_in_is_in_range(uint16_t value)
{
    return (value <= 4095u);
}

uint16_t ct_log2_tcu_log2_tcu_rps_in_encode(double value)
{
    return (uint16_t)(value / 0.025);
}

double ct_log2_tcu_log2_tcu_rps_in_decode(uint16_t value)
{
    return ((double)value * 0.025);
}

bool ct_log2_tcu_log2_tcu_rps_in_is_in_range(uint16_t value)
{
    return (value <= 4095u);
}

uint8_t ct_log2_tcu_log2_tcu_fc_in_encode(double value)
{
    return (uint8_t)(value);
}

double ct_log2_tcu_log2_tcu_fc_in_decode(uint8_t value)
{
    return ((double)value);
}

bool ct_log2_tcu_log2_tcu_fc_in_is_in_range(uint8_t value)
{
    return (value <= 3u);
}

uint8_t ct_log2_tcu_log2_tcu_status_encode(double value)
{
    return (uint8_t)(value);
}

double ct_log2_tcu_log2_tcu_status_decode(uint8_t value)
{
    return ((double)value);
}

bool ct_log2_tcu_log2_tcu_status_is_in_range(uint8_t value)
{
    return (value <= 1u);
}
