
#ifndef __MB_DEF_H__
#define __MB_DEF_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "mb_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief modbus function code
 */
enum mb_fc
{
	MB_FC_01 = (uint8_t)0x01,  //!< 0x01 - read coil
	MB_FC_02 = (uint8_t)0x02,  //!< 0x02 - read discrete input
	MB_FC_03 = (uint8_t)0x03,  //!< 0x03 - read holding register
	MB_FC_04 = (uint8_t)0x04,  //!< 0x04 - read input register
	MB_FC_05 = (uint8_t)0x05,  //!< 0x05 - write single coil
	MB_FC_06 = (uint8_t)0x06,  //!< 0x06 - write single holding register
	MB_FC_0F = (uint8_t)0x0F,  //!< 0x0f - write multiple coils
	MB_FC_10 = (uint8_t)0x10,  //!< 0x0f - write multiple holding register
};

/**
 * @brief modbus transport type
 */
enum mb_transport_type
{
	MB_TRANSPORT_RTU = 0,  //!< Modbus RTU
	MB_TRANSPORT_TCP,      //!< Modbus TCP
	MB_TRANSPORT_ASCII,    //!< Modbus ASCII
};

/**
 * struct mb_master_transport - Modbus master transport interface
 * @type:          Transport type (RTU, TCP, ASCII)
 * @data_transmit: Transmit raw Modbus frame data
 *
 * This structure defines the transport-layer interface used by the
 * Modbus master. Each transport implementation (RTU/TCP/ASCII) must
 * provide a @data_transmit callback to send a complete Modbus frame.
 */
struct mb_transport
{
	enum mb_transport_type type;
	bool (*data_transmit_func)(const struct mb_transport *t, const uint8_t *data, uint32_t length);
};

#ifdef __cplusplus
}
#endif

#endif  //!__MB_DEF_H__
