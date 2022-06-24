#ifndef RBCAN_H
#define RBCAN_H

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <lift/RBSharedMemory.h>
#include <lift/RBDaemon.h>
#include <lift/RBLog.h>
#define RBCAN_MAX_NUM   200


typedef struct _RBCAN_MB_{
    unsigned int    id;         // Identifier
    unsigned char   data[8];    // Data
    unsigned char   dlc;        // Data Length Code
    unsigned char   status;     // MB status
    unsigned char   channel;    // CAN channel (0, 1, 2, ...)
} RBCAN_MB, *pRBCAN_MB;

typedef struct{
    int    is_new_data; // 01
    double gx;          // deg/s
    double gy;
    double gz;
    double ax;          // g
    double ay;
    double az;
    double temp;        // celcius
    int    switch_A;    // 01
    int    switch_B;
    double anal_input_A;// V
    double anal_input_B;
    int    digi_input_A;// 01
    int    digi_input_B;
    int    output_voltage;//V 0 or 12 or 24
    int    digi_output_A;
    int    digi_output_B;
    int    rs485_mode;//0=tx 1 = rx
}TCP_BOARD_DATA_STRUC;
extern TCP_BOARD_DATA_STRUC RCR_EE_DATA;
typedef struct{
    int     is_new_data;
    unsigned int type;
    unsigned int data;
}TCP_BOARD_GENERAL_STURC;
extern TCP_BOARD_GENERAL_STURC RCR_EE_GENERAL;

typedef struct{
    int     is_new_data;
    unsigned int rcv_type;
    unsigned int rcv_data;
}GPIF_SIDE_GERNERAL_DATA_STRUC;
extern GPIF_SIDE_GERNERAL_DATA_STRUC RCR_SIDE_GENERAL;

typedef struct{
    int     is_new_data;
    unsigned int ask_type;
}GPIF_MAIN_GENERAL_ASK_MSG_STRUC;
extern GPIF_MAIN_GENERAL_ASK_MSG_STRUC RCR_MAIN_GENERAL;

typedef struct{
    int     is_new_data;
    unsigned int distance;
    unsigned int signal_quality;
}GPIF_EXTEND_LRF_STRUC;
extern GPIF_EXTEND_LRF_STRUC RCR_EXTEND_LRF;
typedef struct{
    bool    is_new_data;
    double  adc[4];
    unsigned int din[18];
    double  dac[4];
    unsigned int dout[16];
    bool power_48V_in_stat;
    bool power_48V_out_stat;
    bool power_24V_stat;
    bool power_emg_sw_stat;
    bool power_pc_sw_stat;
    bool power_mc_sw_stat;
    bool power_gp_sw_stat;

    bool gpif_adc_init_stat;
    bool gpif_dac_init_stat;
    bool gpif_din_init_stat;
    bool gpif_dout_init_stat;

    double power_48V_amp;

    bool    is_JOG_data;
    int     jog_sig[8];
    int     jog_sig_debug;
}GPIF_RX_DATA_STRUCT;
extern GPIF_RX_DATA_STRUCT RCR_GPIF_RX_DATA;
typedef struct{
    bool    is_new_data;
    unsigned char byte[1];
}GPIF_TX_POWER_STRUCT;
extern GPIF_TX_POWER_STRUCT RCR_GPIF_TX_POWER;

typedef struct{
    bool    is_new_data;
    unsigned char byte[6];
}GPIF_TX_GP_OUT_STRUCT;
extern GPIF_TX_GP_OUT_STRUCT RCR_GPIF_TX_GP_OUT;
typedef struct{
    bool    is_new_Data;
    unsigned char cmd;
}GPIF_TX_CMD_STRUCT;
extern GPIF_TX_CMD_STRUCT RCR_GPIF_TX_CMD;

typedef struct{
    bool    is_new_Data;
    unsigned int addr;
    unsigned int data;
}TCP_BOARD_EEPROM_DATA_STRUCT;
extern TCP_BOARD_EEPROM_DATA_STRUCT RCR_EE_ROM_DATA;

typedef struct{
    bool    is_new_Data;
    unsigned char dat[18];
}GPIF_LCD_MSG_STRUCT;
extern GPIF_LCD_MSG_STRUCT RCR_GPIF_LCD_MSG;
extern GPIF_LCD_MSG_STRUCT RCR_GPIF_IP_MSG;

typedef struct{
    bool    is_new_Data;
    unsigned char rx_flag;
}BOOT_LOADER_STAT_STRUCT;
extern BOOT_LOADER_STAT_STRUCT RCR_BL_STAT[6];

extern double GPIF_TX_SINGLE_DAC[4];
extern unsigned int GPIF_TX_SINGLE_DOUT[16];
extern int IS_SIDE_BOARD_DATA;

extern unsigned char gpif_side_CAN_byte[6];

//#define __GPIF_VERSION_3__
#define __GPIF_VERSION_4__

extern int can_timeout_err[10];

class RBCAN
{
public:
    RBCAN();

    int     RBCAN_WriteData(RBCAN_MB &mb);
    void    RBCAN_SendData();
    void    RBCAN_MessageDecode(int id, unsigned char *data, int dlc);
    void    RBCAN_ClearBuffer();
    int     RBCAN_AddMailBox(unsigned int _id);
    int     RBCAN_GetMailBoxIndex(unsigned int _id);
    int     RBCAN_ReadData(pRBCAN_MB _mb);

    RBCAN_MB		canTotalMB[RBCAN_MAX_NUM];
    int             canTotalIndex;

    int             canHeadIndex;
    int             canTailIndex;

    int             canMBCounter;
    RBCAN_MB        canReadMB[RBCAN_MAX_NUM];

private:



};

#endif // RBCAN_H
