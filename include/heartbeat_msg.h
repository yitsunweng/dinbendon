#ifndef __HEARTBEAT_MSG_H__
#define __HEARTBEAT_MSG_H__
#define CDMAINTERFACE	1
#define	LTEINTERFACE	2
#define WLANINTERFACE	3
#define LV1	"  |__"
#define LV2	"  |    |__"
#define LV3	"  |         |__"
#define LV4	"       |__"
#define HEARTBEAT_WLAN		"(device_id, timestamp, client_num, battery, uptime, loadavg, mem_usage, "\
				"ip_addr, rx, tx, connection, sinr, rssi, bssid, essid, interface)"
#define HEARTBEAT_WITH_IF	"(device_id, timestamp, client_num, battery, uptime, loadavg, mem_usage, "\
				"ip_addr, rx, tx, connection, sinr, rssi, Interface)"
#define HEARTBEAT_WITHOUT_IF	"(device_id, timestamp, client_num, battery, uptime, loadavg, mem_usage)"
#define CLIENT_ITEMS		"(heartbeat_id, timestamp, user_id, client_sid, client_rx, client_tx)"

void heartbeat_msg_handler(char* topic,void* msg, int msg_len);

#endif

