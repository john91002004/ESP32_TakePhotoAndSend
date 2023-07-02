#ifndef APP_HTTPD_H
#define APP_HTTPD_H

#define TWELVE_SEC_IN_MICRO 12000000

#define STOP_STATE 0 
#define START_STATE 2 
#define TAKE_STATE 1 
#define EXIT_STATE -1 
#define WAIT_STATE 3

// for global use 
int wifi_ble_switch_cmd_enable; 
int ble_state = 0; 
int wifi_state; 
int httpd_exit_flag = 0; 
int site_choice = 1; 
int pushButton = 15; 
int LED_RED = 13; 
int LED_GREEN = 12; 
int LED_BLUE = 14; 
int state = START_STATE; 

#endif 