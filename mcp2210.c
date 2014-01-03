/*
 * Copyright 2014 Con Kolivas
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option)
 * any later version.  See COPYING for more details.
 */
#include "miner.h"
#include "usbutils.h"
#include "mcp2210.h"

bool mcp2210_send_recv(struct cgpu_info *cgpu, char *buf, enum usb_cmds cmd)
{
	uint8_t mcp_cmd = buf[0];
	int amount, err;

	if (unlikely(cgpu->usbinfo.nodev))
		return false;

	err = usb_write(cgpu, buf, MCP2210_BUFFER_LENGTH, &amount, cmd);
	if (err || amount != MCP2210_BUFFER_LENGTH) {
		applog(LOG_WARNING, "%s %d: Error %d sending %s sent %d of %d",
		       cgpu->drv->name, cgpu->device_id, err, usb_cmdname(cmd),
		       amount, MCP2210_BUFFER_LENGTH);
		return false;
	}

	err = usb_read(cgpu, buf, MCP2210_BUFFER_LENGTH, &amount, cmd);
	if (err || amount != MCP2210_BUFFER_LENGTH) {
		applog(LOG_WARNING, "%s %d: Error %d receiving %s received %d of %d",
		       cgpu->drv->name, cgpu->device_id, err, usb_cmdname(cmd),
		       amount, MCP2210_BUFFER_LENGTH);
		return false;
	}

	/* Return code should always echo original command */
	if (buf[0] != mcp_cmd) {
		applog(LOG_WARNING, "%s %d: Response code mismatch, asked for %u got %u",
		       cgpu->drv->name, cgpu->device_id, mcp_cmd, buf[0]);
		return false;
	}
	return true;
}