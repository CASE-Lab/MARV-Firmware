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

#ifndef CT_H
#define CT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifndef EINVAL
#    define EINVAL 22
#endif

/* Frame ids. */
#define CT_HEARTBEAT_TCU_FRAME_ID (0x504u)
#define CT_LOG1_TCU_FRAME_ID (0x3c4u)
#define CT_LOG2_TCU_FRAME_ID (0x3e4u)

/* Frame lengths in bytes. */
#define CT_HEARTBEAT_TCU_LENGTH (3u)
#define CT_LOG1_TCU_LENGTH (8u)
#define CT_LOG2_TCU_LENGTH (8u)

/* Extended or standard frame types. */
#define CT_HEARTBEAT_TCU_IS_EXTENDED (0)
#define CT_LOG1_TCU_IS_EXTENDED (0)
#define CT_LOG2_TCU_IS_EXTENDED (0)

/* Frame cycle times in milliseconds. */


/* Signal choices. */
#define CT_HEARTBEAT_TCU_HEARTBEAT_TCU_STATUS_ER_CHOICE (0u)
#define CT_HEARTBEAT_TCU_HEARTBEAT_TCU_STATUS_OK_CHOICE (1u)
#define CT_HEARTBEAT_TCU_HEARTBEAT_TCU_STATUS_NC_CHOICE (2u)

#define CT_HEARTBEAT_TCU_HEARTBEAT_TCU_SOURCE_NONE_CHOICE (0u)
#define CT_HEARTBEAT_TCU_HEARTBEAT_TCU_SOURCE_RCU_CHOICE (1u)
#define CT_HEARTBEAT_TCU_HEARTBEAT_TCU_SOURCE_ACU_CHOICE (2u)
#define CT_HEARTBEAT_TCU_HEARTBEAT_TCU_SOURCE_TESTING_CHOICE (3u)

#define CT_LOG1_TCU_LOG1_TCU_FC_OUT_UNDEFINED_CHOICE (0u)
#define CT_LOG1_TCU_LOG1_TCU_FC_OUT_OPEN_CHOICE (1u)
#define CT_LOG1_TCU_LOG1_TCU_FC_OUT_CLOSED_CHOICE (2u)

#define CT_LOG1_TCU_LOG1_TCU_STATUS_NOT_VALID_CHOICE (0u)
#define CT_LOG1_TCU_LOG1_TCU_STATUS_VALID_CHOICE (1u)

#define CT_LOG2_TCU_LOG2_TCU_FC_IN_UNDEFINED_CHOICE (0u)
#define CT_LOG2_TCU_LOG2_TCU_FC_IN_OPEN_CHOICE (1u)
#define CT_LOG2_TCU_LOG2_TCU_FC_IN_CLOSED_CHOICE (2u)

#define CT_LOG2_TCU_LOG2_TCU_STATUS_NOT_VALID_CHOICE (0u)
#define CT_LOG2_TCU_LOG2_TCU_STATUS_VALID_CHOICE (1u)

/**
 * Signals in message heartbeatTCU.
 *
 * 
 *
 * All signal values are as on the CAN bus.
 */
struct ct_heartbeat_tcu_t {
    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    uint8_t heartbeat_tcu_status;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    uint8_t heartbeat_tcu_error_code;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    uint8_t heartbeat_tcu_source;
};

/**
 * Signals in message log1TCU.
 *
 * APS & RPS written to WaveRunner system
 *
 * All signal values are as on the CAN bus.
 */
struct ct_log1_tcu_t {
    /**
     * Range: -
     * Scale: 2
     * Offset: 0
     */
    uint16_t log1_tcu_aps_raw_out;

    /**
     * Range: -
     * Scale: 2
     * Offset: 0
     */
    uint16_t log1_tcu_rps_raw_out;

    /**
     * Range: -
     * Scale: 2
     * Offset: 0
     */
    uint16_t log1_tcu_fc_raw_out;

    /**
     * Range: -
     * Scale: 0.025
     * Offset: 0
     */
    uint16_t log1_tcu_aps_out;

    /**
     * Range: -
     * Scale: 0.025
     * Offset: 0
     */
    uint16_t log1_tcu_rps_out;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    uint8_t log1_tcu_fc_out;

    /**
     * Valid when 12V Auto is active
     *
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    uint8_t log1_tcu_status;
};

/**
 * Signals in message log2TCU.
 *
 * APS & RPS read from WaveRunner handles
 *
 * All signal values are as on the CAN bus.
 */
struct ct_log2_tcu_t {
    /**
     * Range: -
     * Scale: 2
     * Offset: 0
     */
    uint16_t log2_tcu_aps_raw_in;

    /**
     * Range: -
     * Scale: 2
     * Offset: 0
     */
    uint16_t log2_tcu_rps_raw_in;

    /**
     * Range: -
     * Scale: 2
     * Offset: 0
     */
    uint16_t log2_tcu_fc_raw_in;

    /**
     * Range: -
     * Scale: 0.025
     * Offset: 0
     */
    uint16_t log2_tcu_aps_in;

    /**
     * Range: -
     * Scale: 0.025
     * Offset: 0
     */
    uint16_t log2_tcu_rps_in;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    uint8_t log2_tcu_fc_in;

    /**
     * Valid when 12V Auto is not activated
     *
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    uint8_t log2_tcu_status;
};

/**
 * Pack message heartbeatTCU.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int ct_heartbeat_tcu_pack(
    uint8_t *dst_p,
    const struct ct_heartbeat_tcu_t *src_p,
    size_t size);

/**
 * Unpack message heartbeatTCU.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int ct_heartbeat_tcu_unpack(
    struct ct_heartbeat_tcu_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t ct_heartbeat_tcu_heartbeat_tcu_status_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double ct_heartbeat_tcu_heartbeat_tcu_status_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool ct_heartbeat_tcu_heartbeat_tcu_status_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t ct_heartbeat_tcu_heartbeat_tcu_error_code_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double ct_heartbeat_tcu_heartbeat_tcu_error_code_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool ct_heartbeat_tcu_heartbeat_tcu_error_code_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t ct_heartbeat_tcu_heartbeat_tcu_source_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double ct_heartbeat_tcu_heartbeat_tcu_source_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool ct_heartbeat_tcu_heartbeat_tcu_source_is_in_range(uint8_t value);

/**
 * Pack message log1TCU.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int ct_log1_tcu_pack(
    uint8_t *dst_p,
    const struct ct_log1_tcu_t *src_p,
    size_t size);

/**
 * Unpack message log1TCU.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int ct_log1_tcu_unpack(
    struct ct_log1_tcu_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t ct_log1_tcu_log1_tcu_aps_raw_out_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double ct_log1_tcu_log1_tcu_aps_raw_out_decode(uint16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool ct_log1_tcu_log1_tcu_aps_raw_out_is_in_range(uint16_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t ct_log1_tcu_log1_tcu_rps_raw_out_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double ct_log1_tcu_log1_tcu_rps_raw_out_decode(uint16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool ct_log1_tcu_log1_tcu_rps_raw_out_is_in_range(uint16_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t ct_log1_tcu_log1_tcu_fc_raw_out_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double ct_log1_tcu_log1_tcu_fc_raw_out_decode(uint16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool ct_log1_tcu_log1_tcu_fc_raw_out_is_in_range(uint16_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t ct_log1_tcu_log1_tcu_aps_out_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double ct_log1_tcu_log1_tcu_aps_out_decode(uint16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool ct_log1_tcu_log1_tcu_aps_out_is_in_range(uint16_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t ct_log1_tcu_log1_tcu_rps_out_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double ct_log1_tcu_log1_tcu_rps_out_decode(uint16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool ct_log1_tcu_log1_tcu_rps_out_is_in_range(uint16_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t ct_log1_tcu_log1_tcu_fc_out_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double ct_log1_tcu_log1_tcu_fc_out_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool ct_log1_tcu_log1_tcu_fc_out_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t ct_log1_tcu_log1_tcu_status_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double ct_log1_tcu_log1_tcu_status_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool ct_log1_tcu_log1_tcu_status_is_in_range(uint8_t value);

/**
 * Pack message log2TCU.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int ct_log2_tcu_pack(
    uint8_t *dst_p,
    const struct ct_log2_tcu_t *src_p,
    size_t size);

/**
 * Unpack message log2TCU.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int ct_log2_tcu_unpack(
    struct ct_log2_tcu_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t ct_log2_tcu_log2_tcu_aps_raw_in_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double ct_log2_tcu_log2_tcu_aps_raw_in_decode(uint16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool ct_log2_tcu_log2_tcu_aps_raw_in_is_in_range(uint16_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t ct_log2_tcu_log2_tcu_rps_raw_in_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double ct_log2_tcu_log2_tcu_rps_raw_in_decode(uint16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool ct_log2_tcu_log2_tcu_rps_raw_in_is_in_range(uint16_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t ct_log2_tcu_log2_tcu_fc_raw_in_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double ct_log2_tcu_log2_tcu_fc_raw_in_decode(uint16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool ct_log2_tcu_log2_tcu_fc_raw_in_is_in_range(uint16_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t ct_log2_tcu_log2_tcu_aps_in_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double ct_log2_tcu_log2_tcu_aps_in_decode(uint16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool ct_log2_tcu_log2_tcu_aps_in_is_in_range(uint16_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t ct_log2_tcu_log2_tcu_rps_in_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double ct_log2_tcu_log2_tcu_rps_in_decode(uint16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool ct_log2_tcu_log2_tcu_rps_in_is_in_range(uint16_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t ct_log2_tcu_log2_tcu_fc_in_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double ct_log2_tcu_log2_tcu_fc_in_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool ct_log2_tcu_log2_tcu_fc_in_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t ct_log2_tcu_log2_tcu_status_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double ct_log2_tcu_log2_tcu_status_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool ct_log2_tcu_log2_tcu_status_is_in_range(uint8_t value);


#ifdef __cplusplus
}
#endif

#endif
