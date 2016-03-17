#ifndef ECHO_H
#define ECHO_H
#include<termios.h>
static struct termios stored_settings;
void echo_off(void)
{
	struct termios new_settings;
  	tcgetattr(0,&stored_settings);
    new_settings = stored_settings;
	new_settings.c_lflag &= (~ECHO);
	tcsetattr(0,TCSANOW,&new_settings);
	return;
}
void echo_on(void)
{
	tcsetattr(0,TCSANOW,&stored_settings);
	return;
}
#endif
