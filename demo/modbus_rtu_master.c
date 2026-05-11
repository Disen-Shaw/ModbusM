
#include <unistd.h>
#include <stdio.h>

#include "mb_master.h"

#define DEMO_SLAVE_ADDRESS 0x01

/// your data transmit function.
static inline bool
rs485_data_transmit(const struct mb_transport *t, const uint8_t *data, uint32_t len)
{
	(void)t;
	return true;
}

/// read holding register callback function.
/// this demo just provide 1, you can provide other function you need.
static inline void
read_holding_callback(struct mb_master *obj, uint8_t slave, uint16_t address, uint16_t quantity,
					  const uint16_t *regs)
{
	for (int i = 0; i < quantity; i++)
	{
		printf("received %d register from slave %d: \r\n", quantity, slave);
		printf("REG[%d]: %4.4x", i, regs[i]);
	}
}

struct mb_master modbus_master_instance;
struct mb_transport modbus_master_port = {.type = MB_TRANSPORT_RTU, .data_transmit_func = rs485_data_transmit};
struct mb_master_cbs modbus_master_cbs = {
	.read_holding = read_holding_callback,
};

void
demo_init(void)
{
	mb_master_init(&modbus_master_instance, &modbus_master_port, &modbus_master_cbs);
}

void
demo_run(void)
{
	for (;;)
	{
		// read input register from SLAVE 0x01's 0x1000 ADDRESS
		modbus_master_instance.ops->read_input(&modbus_master_instance, DEMO_SLAVE_ADDRESS, 0x1000,
											   1);
		sleep(1);
	}
}


