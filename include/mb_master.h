
#ifndef __MB_MASTER_H__
#define __MB_MASTER_H__

#include "mb_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

struct mb_master;

/**
 * struct mb_master_ops - Modbus master operations
 * @read_coil:            Function code 0x01 - read coils
 * @read_discrete:        Function code 0x02 - read discrete inputs
 * @read_holding:         Function code 0x03 - read holding registers
 * @read_input:           Function code 0x04 - read input registers
 * @write_single_coil:    Function code 0x05 - write single coil
 * @write_single_holding: Function code 0x06 - write single holding register
 * @write_multiple_coil:  Function code 0x0F - write multiple coils
 * @write_multiple_holding: Function code 0x10 - write multiple holding registers
 */
struct mb_master_ops
{
	bool (*read_coil)(struct mb_master *obj, uint8_t slave, uint16_t address, uint16_t quantity);

	bool (*read_discrete)(struct mb_master *obj, uint8_t slave, uint16_t address,
						  uint16_t quantity);

	bool (*read_holding)(struct mb_master *obj, uint8_t slave, uint16_t address, uint16_t quantity);

	bool (*read_input)(struct mb_master *obj, uint8_t slave, uint16_t address, uint16_t quantity);

	bool (*write_single_coil)(struct mb_master *obj, uint8_t slave, uint16_t address, bool status);

	bool (*write_single_holding)(struct mb_master *obj, uint8_t slave, uint16_t address,
								 uint16_t status);

	bool (*write_multiple_coil)(struct mb_master *obj, uint8_t slave, uint16_t address,
								uint16_t quantity, const bool *status);

	bool (*write_multiple_holding)(struct mb_master *obj, uint8_t slave, uint16_t address,
								   uint16_t quantity, const uint16_t *status);
};

/**
 * struct mb_master_cbs - Modbus master callback functions
 * @read_coil:               Callback for function code 0x01 (read coils)
 * @read_discrete:           Callback for function code 0x02 (read discrete inputs)
 * @read_holding:            Callback for function code 0x03 (read holding registers)
 * @read_input:              Callback for function code 0x04 (read input registers)
 * @write_single_coil:       Callback for function code 0x05 (write single coil)
 * @write_single_holding:    Callback for function code 0x06 (write single holding register)
 * @write_multiple_coil:     Callback for function code 0x0F (write multiple coils)
 * @write_multiple_holding:  Callback for function code 0x10 (write multiple holding registers)
 * @error:                   Callback for Modbus exception responses
 *
 * All callbacks are optional. The @obj parameter is the Modbus master
 * instance invoking the callback.
 */
struct mb_master_cbs
{
	void (*read_coil)(struct mb_master *obj, uint8_t slave, uint16_t address, uint16_t quentity,
					  const bool *status);

	void (*read_discrete)(struct mb_master *obj, uint8_t slave, uint16_t address, uint16_t quentity,
						  const bool *status);

	void (*read_holding)(struct mb_master *obj, uint8_t slave, uint16_t address, uint16_t quentity,
						 const uint16_t *status);

	void (*read_input)(struct mb_master *obj, uint8_t slave, uint16_t address, uint16_t quentity,
					   const uint16_t *status);

	void (*write_single_coil)(struct mb_master *obj, uint8_t slave, uint16_t address, bool status);

	void (*write_single_holding)(struct mb_master *obj, uint8_t slave, uint16_t address,
								 uint16_t status);

	void (*write_multiple_coil)(struct mb_master *obj, uint8_t slave, uint16_t address,
								uint16_t quantity);

	void (*write_multiple_holding)(struct mb_master *obj, uint8_t slave, uint16_t address,
								   uint16_t quentity);

	void (*error)(struct mb_master *obj, uint8_t slave, uint8_t fc, uint8_t error);
};

/**
 * struct mb_master - Modbus master context
 * @on_response:     Callback invoked when a complete Modbus response is received
 * @pending:         Pending request information (slave, function code, address, quantity)
 * @response_buf:    Parsed response buffer (coil or register data)
 * @port:            Transport interface used to transmit Modbus frames
 * @callback:        Application-level callbacks for parsed Modbus data
 * @ops:             Master operation table (read/write functions)
 * @transmit_buffer: Internal transmit buffer
 * @receive_buffer:  Internal receive buffer
 * @receive_count:   Number of bytes currently stored in @receive_buffer
 *
 * This structure represents a Modbus master instance. It contains the
 * transport interface, callback handlers, operation table, and internal
 * buffers required to send requests and process responses.
 */
struct mb_master
{
	void (*on_response)(struct mb_master *self, const uint8_t *data, uint32_t length);

	struct
	{
		uint8_t slave;
		uint8_t fc;
		uint16_t address;
		uint16_t quantity;
	} pending;

	union {
		bool coil_buf[MB_MASTER_COIL_BUF_SIZE];
		uint16_t register_buf[MB_MASTER_REGISTER_BUF_SIZE];
	} response_buf;

	const struct mb_transport *port;
	const struct mb_master_cbs *callback;
	const struct mb_master_ops *ops;

	uint8_t transmit_buffer[MB_MASTER_TX_BUF_SIZE];
	uint8_t receive_buffer[MB_MASTER_RX_BUF_SIZE];
	uint32_t receive_count;
};

bool
mb_master_init(struct mb_master *obj, struct mb_transport *port, struct mb_master_cbs *cbs);

#ifdef __cplusplus
}
#endif

#endif
