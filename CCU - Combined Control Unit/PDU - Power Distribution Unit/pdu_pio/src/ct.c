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
 * This file was generated by cantools version 36.2.0 Fri May 28 12:04:34 2021.
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

int ct_heartbeat_pdu_pack(
    uint8_t *dst_p,
    const struct ct_heartbeat_pdu_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 3);

    dst_p[0] |= pack_left_shift_u8(src_p->heartbeat_pdu_status, 0u, 0xffu);
    dst_p[1] |= pack_left_shift_u8(src_p->heartbeat_pdu_error_code, 0u, 0xffu);
    dst_p[2] |= pack_left_shift_u8(src_p->heartbeat_pdu_source, 0u, 0xffu);

    return (3);
}

int ct_heartbeat_pdu_unpack(
    struct ct_heartbeat_pdu_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    dst_p->heartbeat_pdu_status = unpack_right_shift_u8(src_p[0], 0u, 0xffu);
    dst_p->heartbeat_pdu_error_code = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    dst_p->heartbeat_pdu_source = unpack_right_shift_u8(src_p[2], 0u, 0xffu);

    return (0);
}

uint8_t ct_heartbeat_pdu_heartbeat_pdu_status_encode(double value)
{
    return (uint8_t)(value);
}

double ct_heartbeat_pdu_heartbeat_pdu_status_decode(uint8_t value)
{
    return ((double)value);
}

bool ct_heartbeat_pdu_heartbeat_pdu_status_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t ct_heartbeat_pdu_heartbeat_pdu_error_code_encode(double value)
{
    return (uint8_t)(value);
}

double ct_heartbeat_pdu_heartbeat_pdu_error_code_decode(uint8_t value)
{
    return ((double)value);
}

bool ct_heartbeat_pdu_heartbeat_pdu_error_code_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t ct_heartbeat_pdu_heartbeat_pdu_source_encode(double value)
{
    return (uint8_t)(value);
}

double ct_heartbeat_pdu_heartbeat_pdu_source_decode(uint8_t value)
{
    return ((double)value);
}

bool ct_heartbeat_pdu_heartbeat_pdu_source_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int ct_status12_v_auto_pack(
    uint8_t *dst_p,
    const struct ct_status12_v_auto_t *src_p,
    size_t size)
{
    if (size < 1u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 1);

    dst_p[0] |= pack_left_shift_u8(src_p->status12_v_auto_state, 0u, 0xffu);

    return (1);
}

int ct_status12_v_auto_unpack(
    struct ct_status12_v_auto_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 1u) {
        return (-EINVAL);
    }

    dst_p->status12_v_auto_state = unpack_right_shift_u8(src_p[0], 0u, 0xffu);

    return (0);
}

uint8_t ct_status12_v_auto_status12_v_auto_state_encode(double value)
{
    return (uint8_t)(value);
}

double ct_status12_v_auto_status12_v_auto_state_decode(uint8_t value)
{
    return ((double)value);
}

bool ct_status12_v_auto_status12_v_auto_state_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int ct_log1_pdu_pack(
    uint8_t *dst_p,
    const struct ct_log1_pdu_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_right_shift_u16(src_p->log1_pdu_db_ext_current, 2u, 0xffu);
    dst_p[1] |= pack_left_shift_u16(src_p->log1_pdu_db_ext_current, 6u, 0xc0u);
    dst_p[1] |= pack_right_shift_u16(src_p->log1_pdu_db_int_current, 4u, 0x3fu);
    dst_p[2] |= pack_left_shift_u16(src_p->log1_pdu_db_int_current, 4u, 0xf0u);
    dst_p[2] |= pack_right_shift_u16(src_p->log1_pdu_acu_current, 6u, 0x0fu);
    dst_p[3] |= pack_left_shift_u16(src_p->log1_pdu_acu_current, 2u, 0xfcu);
    dst_p[3] |= pack_right_shift_u16(src_p->log1_pdu_db_int_auto_current, 8u, 0x03u);
    dst_p[4] |= pack_left_shift_u16(src_p->log1_pdu_db_int_auto_current, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(src_p->log1_pdu_db_ext_auto_current, 2u, 0xffu);
    dst_p[6] |= pack_left_shift_u16(src_p->log1_pdu_db_ext_auto_current, 6u, 0xc0u);
    dst_p[7] |= pack_left_shift_u8(src_p->log1_pdu_int_temp, 0u, 0xffu);

    return (8);
}

int ct_log1_pdu_unpack(
    struct ct_log1_pdu_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->log1_pdu_db_ext_current = unpack_left_shift_u16(src_p[0], 2u, 0xffu);
    dst_p->log1_pdu_db_ext_current |= unpack_right_shift_u16(src_p[1], 6u, 0xc0u);
    dst_p->log1_pdu_db_int_current = unpack_left_shift_u16(src_p[1], 4u, 0x3fu);
    dst_p->log1_pdu_db_int_current |= unpack_right_shift_u16(src_p[2], 4u, 0xf0u);
    dst_p->log1_pdu_acu_current = unpack_left_shift_u16(src_p[2], 6u, 0x0fu);
    dst_p->log1_pdu_acu_current |= unpack_right_shift_u16(src_p[3], 2u, 0xfcu);
    dst_p->log1_pdu_db_int_auto_current = unpack_left_shift_u16(src_p[3], 8u, 0x03u);
    dst_p->log1_pdu_db_int_auto_current |= unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    dst_p->log1_pdu_db_ext_auto_current = unpack_left_shift_u16(src_p[5], 2u, 0xffu);
    dst_p->log1_pdu_db_ext_auto_current |= unpack_right_shift_u16(src_p[6], 6u, 0xc0u);
    dst_p->log1_pdu_int_temp = unpack_right_shift_u8(src_p[7], 0u, 0xffu);

    return (0);
}

uint16_t ct_log1_pdu_log1_pdu_db_ext_current_encode(double value)
{
    return (uint16_t)(value / 10.0);
}

double ct_log1_pdu_log1_pdu_db_ext_current_decode(uint16_t value)
{
    return ((double)value * 10.0);
}

bool ct_log1_pdu_log1_pdu_db_ext_current_is_in_range(uint16_t value)
{
    return (value <= 1023u);
}

uint16_t ct_log1_pdu_log1_pdu_db_int_current_encode(double value)
{
    return (uint16_t)(value / 10.0);
}

double ct_log1_pdu_log1_pdu_db_int_current_decode(uint16_t value)
{
    return ((double)value * 10.0);
}

bool ct_log1_pdu_log1_pdu_db_int_current_is_in_range(uint16_t value)
{
    return (value <= 1023u);
}

uint16_t ct_log1_pdu_log1_pdu_acu_current_encode(double value)
{
    return (uint16_t)(value / 10.0);
}

double ct_log1_pdu_log1_pdu_acu_current_decode(uint16_t value)
{
    return ((double)value * 10.0);
}

bool ct_log1_pdu_log1_pdu_acu_current_is_in_range(uint16_t value)
{
    return (value <= 1023u);
}

uint16_t ct_log1_pdu_log1_pdu_db_int_auto_current_encode(double value)
{
    return (uint16_t)(value / 10.0);
}

double ct_log1_pdu_log1_pdu_db_int_auto_current_decode(uint16_t value)
{
    return ((double)value * 10.0);
}

bool ct_log1_pdu_log1_pdu_db_int_auto_current_is_in_range(uint16_t value)
{
    return (value <= 1023u);
}

uint16_t ct_log1_pdu_log1_pdu_db_ext_auto_current_encode(double value)
{
    return (uint16_t)(value / 10.0);
}

double ct_log1_pdu_log1_pdu_db_ext_auto_current_decode(uint16_t value)
{
    return ((double)value * 10.0);
}

bool ct_log1_pdu_log1_pdu_db_ext_auto_current_is_in_range(uint16_t value)
{
    return (value <= 1023u);
}

uint8_t ct_log1_pdu_log1_pdu_int_temp_encode(double value)
{
    return (uint8_t)(value / 2.0);
}

double ct_log1_pdu_log1_pdu_int_temp_decode(uint8_t value)
{
    return ((double)value * 2.0);
}

bool ct_log1_pdu_log1_pdu_int_temp_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}
