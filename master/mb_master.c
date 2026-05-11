
#include "mb_master.h"

#include <string.h>

void
rtu_on_response(struct mb_master *obj, const uint8_t *data, uint32_t length);

extern struct mb_master_ops mb_master_ops_rtu;

bool
mb_master_init(struct mb_master *obj, struct mb_transport *port, struct mb_master_cbs *cbs)
{
	// allow cb as NULL
	if (obj == NULL || port == NULL) { return false; }

	obj->receive_count = 0;
	obj->callback = cbs;
	obj->port = port;

	switch (port->type)
	{
		case MB_TRANSPORT_RTU:
			obj->ops = &mb_master_ops_rtu;
			obj->on_response = rtu_on_response;
			break;
		default:
			return false;
	}
	return true;
}


