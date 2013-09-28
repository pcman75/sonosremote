#ifndef CONFIG_H
#define CONFIG_H

#define SONOSREMOTE_WEBSEVICE "http://sonospebbleremote.appspot.com"
//#define SONOSREMOTE_WEBSEVICE "192.168.1.104:8080"
//#define SONOSREMOTE_WEBSEVICE "http://bfree.ro/sonos/SonosPebble.php"
	
#define SONOSREMOTE_APP_ID 2813748771
#define SONOSREMOTE_HTTP_COOKIE 726128123

//commands:	
#define SONOSREMOTE_KEY_COMMAND 1
#define SONOSREMOTE_KEY_COMMAND_SUCCESS 2
#define SONOSREMOTE_KEY_TRACK_INFO_ARTIST 3
#define SONOSREMOTE_KEY_TRACK_INFO_ALBUM 4
#define SONOSREMOTE_KEY_TRACK_INFO_TITLE 5
#define SONOSREMOTE_KEY_TRACK_INFO_POSITION 6
#define SONOSREMOTE_KEY_TRACK_INFO_ALBUM_ART 7
	
#define SONOSREMOTE_CMD_PLAY 1
#define SONOSREMOTE_CMD_STOP 2
#define SONOSREMOTE_CMD_PAUSE 3
#define SONOSREMOTE_CMD_PREV 4
#define SONOSREMOTE_CMD_NEXT 5
#define SONOSREMOTE_CMD_INFO 6
#define SONOSREMOTE_CMD_MUTE 7
	
#endif // CONFIG_H