#ifndef __I2C_H__
#define __I2C_H__

typedef enum 
{
    I2C0 = 0,
    I2C1,
    I2C2

}I2C_INSTANCE_ID;


//I2C registers
#define  I2C_REVNB_LO                   0x00000000
#define  I2C_REVNB_HI                   0x00000004
#define  I2C_SYSC                       0x00000010
#define  I2C_IRQSTATUS_RAW              0x00000024
#define  I2C_IRQSTATUS                  0x00000028
#define  I2C_IRQENABLE_SET              0x0000002C
#define  I2C_IRQENABLE_CLR              0x00000030
#define  I2C_WE                         0x00000034
#define  I2C_DMARXENABLE_SET            0x00000038
#define  I2C_DMATXENABLE_SET            0x0000003C
#define  I2C_DMARXENABLE_CLR            0x00000040
#define  I2C_DMATXENABLE_CLR            0x00000044
#define  I2C_DMARXWAKE_EN               0x00000048
#define  I2C_DMATXWAKE_EN               0x0000004C
#define  I2C_SYSS                       0x00000090
#define  I2C_BUF                        0x00000094
#define  I2C_CNT                        0x00000098
#define  I2C_DATA                       0x0000009C
#define  I2C_CON                        0x000000A4
#define  I2C_OA                         0x000000A8
#define  I2C_SA                         0x000000AC
#define  I2C_PSC                        0x000000B0
#define  I2C_SCLL                       0x000000B4
#define  I2C_SCLH                       0x000000B8
#define  I2C_SYSTEST                    0x000000BC
#define  I2C_BUFSTAT                    0x000000C0
#define  I2C_OA1                        0x000000C4
#define  I2C_OA2                        0x000000C8
#define  I2C_OA3                        0x000000CC
#define  I2C_ACTOA                      0x000000D0
#define  I2C_SBLOCK                     0x000000D4




#endif
