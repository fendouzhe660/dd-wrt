/*
 * bridgetools.c
 *
 * Copyright (C) 2007 Sebastian Gottschall <gottschall@dd-wrt.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Id:
 */
#include <stdlib.h>
#include <bcmnvram.h>
#include <shutils.h>
#include <utils.h>
#include <syslog.h>
#ifdef HAVE_MICRO

int brctl_main(int argc, char **argv)
{
	if (argc == 1) {
		fprintf(stderr, "try to be professional!\n");
		return -1;
	}
	br_init();
	if (!strcmp(argv[1], "addif")) {
		if (ifexists(argv[3]))
			br_add_interface(argv[2], argv[3]);
	}
	if (!strcmp(argv[1], "delif")) {
		if (ifexists(argv[3]))
			br_del_interface(argv[2], argv[3]);
	}
	if (!strcmp(argv[1], "addbr")) {
		br_add_bridge(argv[2]);
	}
	if (!strcmp(argv[1], "stp")) {
		br_set_stp_state(argv[2], atoi(argv[3]));
	}
	if (!strcmp(argv[1], "delbr")) {
		if (!ifexists(argv[2]))
			return -1;
		br_del_bridge(argv[2]);
	}
	if (!strcmp(argv[1], "show")) {
		br_cmd_show();
	}
	br_shutdown();
}
#else

#ifdef HAVE_MSTP

int br_set_stp_state(const char *br, int stp_state)
{
	if (!ifexists(br))
		return -1;
	if (stp_state == 1) {
		// syslog (LOG_INFO, "stp is set to on\n");
		return eval("brctl", "stp", br, "1");
	} else {
		// syslog (LOG_INFO, "stp is set to off\n");
		return eval("brctl", "stp", br, "0");
	}
}

int br_set_port_prio(const char *br, char *port, char *prio)
{
	if (!ifexists(br))
		return -1;
	return eval("mstpctl", "settreeportprio", br, port, "0", prio);
}

int br_set_bridge_prio(const char *br, char *prio)
{
	if (!ifexists(br))
		return -1;
	return eval("mstpctl", "settreeprio", br, "0", prio);
}

int br_set_bridge_forward_delay(const char *br, int sec)
{
	char delay[32];

	sprintf(delay, "%d", sec);
	return eval("mstpctl", "setfdelay", br, delay);

}
#else
int br_set_bridge_forward_delay(const char *br, int sec)
{
	char delay[32];

	sprintf(delay, "%d", sec);
	return eval("brctl", "setfd", br, delay);
}

int br_set_stp_state(const char *br, int stp_state)
{
	if (!ifexists(br))
		return -1;
	if (stp_state == 1) {
		// syslog (LOG_INFO, "stp is set to on\n");
		return eval("brctl", "stp", br, "1");
	} else {
		// syslog (LOG_INFO, "stp is set to off\n");
		return eval("brctl", "stp", br, "0");
	}
}

int br_set_port_prio(const char *br, char *port, char *prio)
{
	if (!ifexists(br))
		return -1;
	return eval("brctl", "setportprio", br, port, prio);
}

int br_set_bridge_prio(const char *br, char *prio)
{
	if (!ifexists(br))
		return -1;
	return eval("brctl", "setbridgeprio", br, prio);
}

#endif
int br_add_bridge(const char *brname)
{
	dd_syslog(LOG_INFO, "bridge added successfully\n");
	char ipaddr[32];

	sprintf(ipaddr, "%s_ipaddr", brname);
	char netmask[32];

	sprintf(netmask, "%s_netmask", brname);
	int ret = eval("brctl", "addbr", brname);

	if (nvram_get(ipaddr) && nvram_get(netmask)
	    && !nvram_match(ipaddr, "0.0.0.0")
	    && !nvram_match(netmask, "0.0.0.0")) {
		eval("ifconfig", brname, nvram_safe_get(ipaddr), "netmask", nvram_safe_get(netmask), "mtu", getBridgeMTU(brname), "up");
	} else
		eval("ifconfig", brname, "mtu", getBridgeMTU(brname));
	return ret;
}

int br_del_bridge(const char *brname)
{
	if (!ifexists(brname))
		return -1;
	dd_syslog(LOG_INFO, "bridge deleted successfully\n");
	return eval("brctl", "delbr", brname);
}

int br_add_interface(const char *br, const char *dev)
{
	if (!ifexists(dev))
		return -1;
	char ipaddr[32];

	sprintf(ipaddr, "%s_ipaddr", dev);
	char netmask[32];

	sprintf(netmask, "%s_netmask", dev);

	eval("ifconfig", dev, "0.0.0.0");
	if (strncmp(dev, "ath", 3) != 0) {	// this is not an ethernet driver
		eval("ifconfig", dev, "down");	//fixup for some ethernet drivers
	}
	eval("ifconfig", dev, "mtu", getBridgeMTU(br));
	if (strncmp(dev, "ath", 3) != 0) {	// this is not an ethernet driver
		eval("ifconfig", dev, "up");
	}

	dd_syslog(LOG_INFO, "interface added successfully\n");
	return eval("brctl", "addif", br, dev);
}

int br_del_interface(const char *br, const char *dev)
{
	if (!ifexists(dev))
		return -1;
	dd_syslog(LOG_INFO, "interface deleted successfully\n");
	return eval("brctl", "delif", br, dev);
}

#endif
