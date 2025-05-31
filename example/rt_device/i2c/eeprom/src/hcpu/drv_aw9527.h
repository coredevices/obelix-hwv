#pragma once

typedef enum {
	IOEXP_LOW,
    IOEXP_HIGH,
}IOEXP_STATE_T;

typedef enum {
    IOEXP_CH00 = 0,
    IOEXP_CH01,
    IOEXP_CH02,
    IOEXP_CH03,
    IOEXP_CH04,
    IOEXP_CH05,
    IOEXP_CH06,
    IOEXP_CH07,
    IOEXP_CH10 = 10,
    IOEXP_CH11,
    IOEXP_CH12,
    IOEXP_CH13,
    IOEXP_CH14,
    IOEXP_CH15,
    IOEXP_CH16,
    IOEXP_CH17,
}IOEXP_CHANNEL_T;

void ioexp_init(void);
/*channel ctrl*/
void ioexp_pin_set(IOEXP_CHANNEL_T channel, IOEXP_STATE_T state); 

