#ifndef _TIMER_STRUCTS_H
#define _TIMER_STRUCTS_H

#include <inttypes.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define PORT 7372	//The port on which to listen for incoming data

typedef struct _pre_msg{

} pre_msg;

/**
 * @brief time data structure
 * 
 */
typedef struct _d_time{
    uint8_t hour;
    uint8_t minutes;
    uint8_t seconds;
    
} __attribute__ ((__packed__)) d_time;


/**
 * @brief 
 * command: setup msg for uptimer
 *          0xA2 -> use uptimer
 *          0xA4 -> uptimer restart
 * display mode:
 *          0x00 -> MIN:SEC
 *          0x01 -> HH:MM:SS
 * none:    
 *          0x00 -> For uptimer
 * 
 * NOTE: Info pkg is : 0xA1 0x04 0xB2
 * 
 */
typedef struct _clock_ctl{
    uint8_t command;
    uint8_t displaymode;
    uint8_t none;
} __attribute__ ((__packed__)) clock_ctl;


/**
 * @brief Control message 
 * command: 
 *          0xA3 -> uptimer start/pause
 *          0xA6 -> downtimer start/pause
 *          0xA8 -> regular Clock setup (need run=0x01)
 * run:
 *          0x00 -> Pause
 *          0x01 -> Count (up/down)
 * none:    
 *          0x00 -> none
 * 
 */
typedef struct _clock_use{
    uint8_t command; 
    uint8_t run;
    uint8_t none;
} __attribute__ ((__packed__)) clock_use;



/**
 * @brief  Setup message for downtimer
 * command: 
 *          0xA5 -> Use downtimer
 *          0xA7 -> restart downtimer
 * displaymode:
 *          0x00 -> MIN:SEC
 *          0x01 -> HH:MM:SS
 * timedisplay  -> HH:MM:SS:ss
 * alarm:
 *          0x00 -> disable.
 *          0x01 -> enable.
 * alarm_dur:  alarm duration on seconds.
 *          
 */
typedef struct _clock_d_ctl{
    uint8_t command;
    uint8_t displaymode; 
    d_time  displaytime;
    uint8_t start_t_sec;  
    uint8_t alarm;     
    uint8_t alarm_dur; 
} __attribute__ ((__packed__)) clock_d_ctl;

/**
 * @brief Control time while clock is running
 *        command must be _ 0xAB
 */
typedef struct _clock_r_ctl{
    uint8_t command;
    d_time  displaytime;
    uint8_t new_t_sec;  
    uint8_t new_h_sec;     
}__attribute__ ((__packed__)) clock_r_ctl;


/**
 * @brief Response package: 35 bytes  --- bytes 21-35 empty
 * Device Type: 0x01=POE, 0x02=WiFi, 0x03=DotMatrix
 * client IP address
 * MAC address
 * firmware version Major:Minor
 * NTP Sync Count
 * Displayed Time: HH MM SS in each byte
 * Device Name as null terminated ASCII string
 */
typedef struct _tmach_response{
    uint8_t devicetype;
    uint32_t clientip;
    uint8_t macaddr[6];
    uint16_t firm_vers;
    uint16_t ntp_sync_count;
    d_time display_time;
    uint8_t deviceName[3];
}   __attribute__ ((__packed__)) tmach_response;



int createSocket();

tmach_response clock_info(int sockfd , char* address);

int clock_setup(int sockfd , char* address , char* reset,
                char* hours, char* mins, char* sec, char* alarm, char* al_dur);

int clock_start_pause(int sockfd , char* address , char* mode );

int clock_running(int sockfd , char* address, char* hours, char* mins,
                               char* sec, char* t_sec, char* h_sec);

#endif