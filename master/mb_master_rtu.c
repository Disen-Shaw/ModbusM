
#include <string.h>

#include "mb_crc.h"
#include "mb_master.h"

static bool
rtu_read_coil(struct mb_master *obj, uint8_t slave, uint16_t address, uint16_t quantity)
{
	uint16_t crc;
	bool ret = false;
	uint8_t *buf = obj->transmit_buffer;

	buf[0] = slave;
	buf[1] = MB_FC_01;
	buf[2] = (address >> 8) & 0xFF;
	buf[3] = address & 0xFF;
	buf[4] = (quantity >> 8) & 0xFF;
	buf[5] = quantity & 0xFF;
	crc = mb_crc16(buf, 6);
	buf[6] = crc & 0xFF;
	buf[7] = (crc >> 8) & 0xFF;

	ret = obj->port->data_transmit_func(obj->port, buf, 8);
	if (ret)
	{
		obj->pending.slave = slave;
		obj->pending.fc = MB_FC_01;
		obj->pending.address = address;
		obj->pending.quantity = quantity;
	}

	return ret;
}

static bool
rtu_read_discrete(struct mb_master *obj, uint8_t slave, uint16_t address, uint16_t quantity)
{
	uint16_t crc;
	bool ret = false;
	uint8_t *buf = obj->transmit_buffer;

	buf[0] = slave;
	buf[1] = MB_FC_02;
	buf[2] = (address >> 8) & 0xFF;
	buf[3] = address & 0xFF;
	buf[4] = (quantity >> 8) & 0xFF;
	buf[5] = quantity & 0xFF;
	crc = mb_crc16(buf, 6);
	buf[6] = crc & 0xFF;
	buf[7] = (crc >> 8) & 0xFF;

	ret = obj->port->data_transmit_func(obj->port, buf, 8);
	if (ret)
	{
		obj->pending.slave = slave;
		obj->pending.fc = MB_FC_02;
		obj->pending.address = address;
		obj->pending.quantity = quantity;
	}

	return ret;
}

static bool
rtu_read_holding(struct mb_master *obj, uint8_t slave, uint16_t address, uint16_t quantity)
{
	uint16_t crc;
	bool ret = false;
	uint8_t *buf = obj->transmit_buffer;

	buf[0] = slave;
	buf[1] = MB_FC_03;
	buf[2] = (address >> 8) & 0xFF;
	buf[3] = address & 0xFF;
	buf[4] = (quantity >> 8) & 0xFF;
	buf[5] = quantity & 0xFF;
	crc = mb_crc16(buf, 6);
	buf[6] = crc & 0xFF;
	buf[7] = (crc >> 8) & 0xFF;

	ret = obj->port->data_transmit_func(obj->port, buf, 8);
	if (ret)
	{
		obj->pending.slave = slave;
		obj->pending.fc = MB_FC_03;
		obj->pending.address = address;
		obj->pending.quantity = quantity;
	}

	return ret;
}

static bool
rtu_read_input(struct mb_master *obj, uint8_t slave, uint16_t address, uint16_t quantity)
{
	uint16_t crc;
	bool ret = false;
	uint8_t *buf = obj->transmit_buffer;

	buf[0] = slave;
	buf[1] = MB_FC_04;
	buf[2] = (address >> 8) & 0xFF;
	buf[3] = address & 0xFF;
	buf[4] = (quantity >> 8) & 0xFF;
	buf[5] = quantity & 0xFF;
	crc = mb_crc16(buf, 6);
	buf[6] = crc & 0xFF;
	buf[7] = (crc >> 8) & 0xFF;

	ret = obj->port->data_transmit_func(obj->port, buf, 8);
	if (ret)
	{
		obj->pending.slave = slave;
		obj->pending.fc = MB_FC_04;
		obj->pending.address = address;
		obj->pending.quantity = quantity;
	}

	return ret;
}

static bool
rtu_write_single_coil(struct mb_master *obj, uint8_t slave, uint16_t address, bool status)
{
	uint16_t crc;
	bool ret = false;
	uint8_t *buf = obj->transmit_buffer;
	uint16_t coil_value;

	buf[0] = slave;
	buf[1] = MB_FC_05;
	buf[2] = (address >> 8) & 0xFF;
	buf[3] = address & 0xFF;
	coil_value = status ? 0xFF00 : 0x0000;
	buf[4] = (coil_value >> 8) & 0xFF;
	buf[5] = coil_value & 0xFF;
	crc = mb_crc16(buf, 6);
	buf[6] = crc & 0xFF;
	buf[7] = (crc >> 8) & 0xFF;

	ret = obj->port->data_transmit_func(obj->port, buf, 8);
	if (ret)
	{
		obj->pending.slave = slave;
		obj->pending.fc = MB_FC_05;
		obj->pending.address = address;
	}

	return ret;
}

static bool
rtu_write_single_holding(struct mb_master *obj, uint8_t slave, uint16_t address, uint16_t status)
{
	uint16_t crc;
	bool ret = false;
	uint8_t *buf = obj->transmit_buffer;

	buf[0] = slave;
	buf[1] = MB_FC_06;
	buf[2] = (address >> 8) & 0xFF;
	buf[3] = address & 0xFF;
	buf[4] = (status >> 8) & 0xFF;
	buf[5] = status & 0xFF;
	crc = mb_crc16(buf, 6);
	buf[6] = crc & 0xFF;
	buf[7] = (crc >> 8) & 0xFF;

	ret = obj->port->data_transmit_func(obj->port, buf, 8);
	if (ret)
	{
		obj->pending.slave = slave;
		obj->pending.fc = MB_FC_06;
		obj->pending.address = address;
	}

	return ret;
}

static bool
rtu_write_multiple_coil(struct mb_master *obj, uint8_t slave, uint16_t address, uint16_t quantity,
						const bool *status)
{
	uint16_t crc;
	uint16_t byte_index;
	uint16_t bit_index;
	uint16_t frame_len_no_crc;
	uint16_t total_len;
	bool ret = false;
	uint8_t byte_count;
	uint8_t *buf = obj->transmit_buffer;

	if (quantity == 0 || quantity > MB_MASTER_COIL_BUF_SIZE) { return false; }

	buf[0] = slave;
	buf[1] = MB_FC_0F;
	buf[2] = (address >> 8) & 0xFF;
	buf[3] = address & 0xFF;
	buf[4] = (quantity >> 8) & 0xFF;
	buf[5] = quantity & 0xFF;
	byte_count = ((quantity + 7) >> 3);
	buf[6] = byte_count;

	memset(&buf[7], 0, byte_count);
	for (int i = 0; i < quantity; i++)
	{
		byte_index = i >> 3;
		bit_index = i & 0x07;
		if (status[i])
		{
			buf[7 + byte_index] |= (1 << bit_index);
		} else
		{
			buf[7 + byte_index] &= ~(1 << bit_index);
		}
	}

	frame_len_no_crc = 7 + byte_count;
	crc = mb_crc16(buf, frame_len_no_crc);
	buf[frame_len_no_crc] = crc & 0xFF;
	buf[frame_len_no_crc + 1] = (crc >> 8) & 0xFF;
	total_len = frame_len_no_crc + 2;
	ret = obj->port->data_transmit_func(obj->port, buf, total_len);

	if (ret)
	{
		obj->pending.slave = slave;
		obj->pending.fc = MB_FC_0F;
		obj->pending.address = address;
		obj->pending.quantity = quantity;
	}

	return ret;
}

static bool
rtu_write_multiple_holding(struct mb_master *obj, uint8_t slave, uint16_t address,
						   uint16_t quantity, const uint16_t *status)
{
	uint16_t crc;
	uint8_t *buf = obj->transmit_buffer;
	bool ret = false;

	if (quantity == 0 || quantity > MB_MASTER_REGISTER_BUF_SIZE) { return false; }

	buf[0] = slave;
	buf[1] = MB_FC_10;
	buf[2] = (address >> 8) & 0xFF;
	buf[3] = address & 0xFF;
	buf[4] = (quantity >> 8) & 0xFF;
	buf[5] = quantity & 0xFF;

	uint8_t byte_count = quantity * 2;
	buf[6] = byte_count;

	for (int i = 0; i < quantity; i++)
	{
		uint16_t reg = status[i];
		buf[7 + i * 2] = (reg >> 8) & 0xFF;
		buf[7 + i * 2 + 1] = reg & 0xFF;
	}

	uint16_t frame_len_no_crc = 7 + byte_count;
	crc = mb_crc16(buf, frame_len_no_crc);
	buf[frame_len_no_crc] = crc & 0xFF;
	buf[frame_len_no_crc + 1] = (crc >> 8) & 0xFF;

	uint16_t total_len = frame_len_no_crc + 2;
	ret = obj->port->data_transmit_func(obj->port, buf, total_len);
	if (ret)
	{
		obj->pending.slave = slave;
		obj->pending.fc = MB_FC_10;
		obj->pending.address = address;
		obj->pending.quantity = quantity;
	}

	return ret;
}

static void
rtu_callback_fc01(struct mb_master *obj, const uint8_t *frame, uint32_t len)
{
	uint8_t slave = frame[0];
	uint8_t byte_count = frame[2];
	bool *coils = obj->response_buf.coil_buf;

	const uint8_t *data = &frame[3];
	uint16_t quantity = obj->pending.quantity;

	if (byte_count < ((quantity + 7) >> 3)) { return; }

	for (uint16_t i = 0; i < quantity; i++)
	{
		uint16_t byte_index = i >> 3;
		uint16_t bit_index = i & 0x07;
		bool coil = (data[byte_index] >> bit_index) & 0x01;
		coils[i] = coil;
	}

	if (obj->callback && obj->callback->read_coil)
	{
		obj->callback->read_coil(obj, slave, obj->pending.address, quantity, coils);
	}
}

static void
rtu_callback_fc02(struct mb_master *obj, const uint8_t *frame, uint32_t len)
{
	uint8_t slave = frame[0];
	uint8_t byte_count = frame[2];
	bool *coils = obj->response_buf.coil_buf;

	const uint8_t *data = &frame[3];
	uint16_t quantity = obj->pending.quantity;

	if (byte_count < ((quantity + 7) >> 3)) { return; }

	for (uint16_t i = 0; i < quantity; i++)
	{
		uint16_t byte_index = i >> 3;
		uint16_t bit_index = i & 0x07;
		bool coil = (data[byte_index] >> bit_index) & 0x01;
		coils[i] = coil;
	}

	if (obj->callback && obj->callback->read_discrete)
	{
		obj->callback->read_discrete(obj, slave, obj->pending.address, quantity, coils);
	}
}

static void
rtu_callback_fc03(struct mb_master *obj, const uint8_t *frame, uint32_t len)
{
	uint8_t slave = frame[0];
	uint8_t byte_count = frame[2];
	const uint8_t *data = &frame[3];

	uint16_t quantity = obj->pending.quantity;
	uint16_t *regs = obj->response_buf.register_buf;

	if (len < 3 + byte_count + 2) return;

	if (byte_count != quantity * 2) return;

	for (uint16_t i = 0; i < quantity; i++)
	{
		uint16_t hi = data[i * 2];
		uint16_t lo = data[i * 2 + 1];
		regs[i] = (hi << 8) | lo;
	}

	if (obj->callback && obj->callback->read_holding)
	{
		obj->callback->read_holding(obj, slave, obj->pending.address, quantity, regs);
	}
}

static void
rtu_callback_fc04(struct mb_master *obj, const uint8_t *frame, uint32_t len)
{
	uint8_t slave = frame[0];
	uint8_t byte_count = frame[2];
	const uint8_t *data = &frame[3];

	uint16_t quantity = obj->pending.quantity;
	uint16_t *regs = obj->response_buf.register_buf;

	if (len < 3 + byte_count + 2) return;

	if (byte_count != quantity * 2) return;

	for (uint16_t i = 0; i < quantity; i++)
	{
		uint16_t hi = data[i * 2];
		uint16_t lo = data[i * 2 + 1];
		regs[i] = (hi << 8) | lo;
	}

	if (obj->callback && obj->callback->read_input)
	{
		obj->callback->read_input(obj, slave, obj->pending.address, quantity, regs);
	}
}

static void
rtu_callback_fc05(struct mb_master *obj, const uint8_t *frame, uint32_t len)
{
	uint8_t slave = frame[0];

	uint16_t address = ((uint16_t)frame[2] << 8) | frame[3];
	uint16_t raw = ((uint16_t)frame[4] << 8) | frame[5];

	bool status;
	if (raw == 0xFF00)
	{
		status = true;
	} else if (raw == 0x0000)
	{
		status = false;
	} else
	{
		return;
	}

	if (obj->callback && obj->callback->write_single_coil)
	{
		obj->callback->write_single_coil(obj, slave, address, status);
	}
}

static void
rtu_callback_fc06(struct mb_master *obj, const uint8_t *frame, uint32_t len)
{
	uint8_t slave = frame[0];
	uint16_t address = ((uint16_t)frame[2] << 8) | frame[3];
	uint16_t value = ((uint16_t)frame[4] << 8) | frame[5];
	if (obj->callback && obj->callback->write_single_holding)
	{
		obj->callback->write_single_holding(obj, slave, address, value);
	}
}

static void
rtu_callback_fc0f(struct mb_master *obj, const uint8_t *frame, uint32_t len)
{
	uint8_t slave = frame[0];
	uint16_t address = ((uint16_t)frame[2] << 8) | frame[3];
	uint16_t quantity = ((uint16_t)frame[4] << 8) | frame[5];
	if (obj->callback && obj->callback->write_multiple_coil)
	{
		obj->callback->write_multiple_coil(obj, slave, address, quantity);
	}
}

static void
rtu_callback_fc10(struct mb_master *obj, const uint8_t *frame, uint32_t len)

{
	uint8_t slave = frame[0];
	uint16_t address = ((uint16_t)frame[2] << 8) | frame[3];
	uint16_t quantity = ((uint16_t)frame[4] << 8) | frame[5];

	if (obj->callback && obj->callback->write_multiple_holding)
	{
		obj->callback->write_multiple_holding(obj, slave, address, quantity);
	}
}

void
rtu_on_response(struct mb_master *obj, const uint8_t *data, uint32_t len)
{
	if (len == 0) return;

	if (obj->receive_count + len > MB_MASTER_RX_BUF_SIZE)
	{
		obj->receive_count = 0;
		return;
	}

	memcpy(&obj->receive_buffer[obj->receive_count], data, len);
	obj->receive_count += len;

	while (obj->receive_count >= 5)
	{
		uint8_t *buf = obj->receive_buffer;
		uint8_t slave = buf[0];
		uint8_t fc = buf[1];

		if (obj->pending.slave != 0 && slave != obj->pending.slave)
		{
			obj->receive_count = 0;
			return;
		}

		uint16_t expected_len = 0;

		if (fc <= MB_FC_04)
		{
			if (obj->receive_count < 3) return;
			uint8_t byte_count = buf[2];
			expected_len = 3 + byte_count + 2;
		} else if (fc == MB_FC_05 || fc == MB_FC_06 || fc == MB_FC_0F || fc == MB_FC_10)
		{
			expected_len = 8;
		} else if (fc & 0x80)
		{
			expected_len = 5;
		} else
		{
			memmove(buf, buf + 1, obj->receive_count - 1);
			obj->receive_count -= 1;
			continue;
		}

		if (expected_len > MB_MASTER_RX_BUF_SIZE)
		{
			memmove(buf, buf + 1, obj->receive_count - 1);
			obj->receive_count -= 1;
			continue;
		}

		if (obj->receive_count < expected_len) return;

		uint16_t crc_calc = mb_crc16(buf, expected_len - 2);
		uint16_t crc_recv =
			(uint16_t)buf[expected_len - 2] | ((uint16_t)buf[expected_len - 1] << 8);

		if (crc_calc != crc_recv)
		{
			memmove(buf, buf + 1, obj->receive_count - 1);
			obj->receive_count -= 1;
			continue;
		}

		if (fc & 0x80)
		{
			uint8_t exception_code = buf[2];
			if (obj->callback && obj->callback->error)
			{
				obj->callback->error(obj, slave, (uint8_t)(fc ^ 0x80), exception_code);
			}
			uint32_t remain = obj->receive_count - expected_len;
			if (remain > 0) memmove(buf, buf + expected_len, remain);
			obj->receive_count = remain;
			continue;
		}

		switch (fc)
		{
			case MB_FC_01:
				rtu_callback_fc01(obj, buf, expected_len);
				break;
			case MB_FC_02:
				rtu_callback_fc02(obj, buf, expected_len);
				break;
			case MB_FC_03:
				rtu_callback_fc03(obj, buf, expected_len);
				break;
			case MB_FC_04:
				rtu_callback_fc04(obj, buf, expected_len);
				break;
			case MB_FC_05:
				rtu_callback_fc05(obj, buf, expected_len);
				break;
			case MB_FC_06:
				rtu_callback_fc06(obj, buf, expected_len);
				break;
			case MB_FC_0F:
				rtu_callback_fc0f(obj, buf, expected_len);
				break;
			case MB_FC_10:
				rtu_callback_fc10(obj, buf, expected_len);
				break;
			default:
				memmove(buf, buf + 1, obj->receive_count - 1);
				obj->receive_count -= 1;
				continue;
		}

		{
			uint32_t remain = obj->receive_count - expected_len;
			if (remain > 0) memmove(buf, buf + expected_len, remain);
			obj->receive_count = remain;
		}
	}
}

struct mb_master_ops mb_master_ops_rtu = {
	.read_coil = rtu_read_coil,
	.read_discrete = rtu_read_discrete,
	.read_holding = rtu_read_holding,
	.read_input = rtu_read_input,
	.write_single_coil = rtu_write_single_coil,
	.write_single_holding = rtu_write_single_holding,
	.write_multiple_coil = rtu_write_multiple_coil,
	.write_multiple_holding = rtu_write_multiple_holding,
};
