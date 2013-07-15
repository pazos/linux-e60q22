#ifndef _BOARD_MX6SL_NTX_H
#define _BOARD_MX6SL_NTX_H
#include <mach/iomux-mx6sl.h>


#define MX6SL_NTX_ACIN_PG		gMX6SL_NTX_ACIN_PG	
#define MX6SL_NTX_CHG			gMX6SL_NTX_CHG	
#define MX6SL_MSP_INT			gMX6SL_MSP_INT
#define MX6SL_PWR_SW			gMX6SL_PWR_SW
#define MX6SL_IR_TOUCH_INT		gMX6SL_IR_TOUCH_INT
#define MX6SL_IR_TOUCH_RST		gMX6SL_IR_TOUCH_RST
#define MX6SL_HALL_EN			gMX6SL_HALL_EN
#define MX6SL_ON_LED			gMX6SL_ON_LED
#define MX6SL_CHG_LED			gMX6SL_CHG_LED
#define MX6SL_ACT_LED			gMX6SL_ACT_LED

#define MX6SL_EXT_SD_CD			IMX_GPIO_NR(5, 2)	/* SD2_DAT4 */
#define MX6SL_WIFI_3V3			IMX_GPIO_NR(5, 0)	/* SD2_DAT7 */
#define MX6SL_WIFI_RST			IMX_GPIO_NR(4, 27)	/* SD2_RST */
#define MX6SL_WIFI_INT			IMX_GPIO_NR(4, 29)	/* SD2_DAT6 */
#define MX6SL_HW_ID0			IMX_GPIO_NR(4, 2)	/* KEY_COL5 */
#define MX6SL_HW_ID1			IMX_GPIO_NR(4, 4)	/* KEY_COL6 */
#define MX6SL_HW_ID2			IMX_GPIO_NR(4, 6)	/* KEY_COL7 */
#define MX6SL_HW_ID3			IMX_GPIO_NR(4, 5)	/* KEY_ROW6 */
#define MX6SL_HW_ID4			IMX_GPIO_NR(4, 7)	/* KEY_ROW7 */
#define MX6SL_FL_EN				IMX_GPIO_NR(2, 10)	/* EPDC_PWRCTRL3 */
#define MX6SL_FL_R_EN			IMX_GPIO_NR(1, 29)	/* EPDC_SDCE2 */
#define MX6SL_EP_PWRALL			IMX_GPIO_NR(2, 14)	/* EPDC_PWRWAKEUP */
#define MX6SL_EP_WAKEUP			IMX_GPIO_NR(2, 7)	/* EPDC_PWRCTRL0 */
#define MX6SL_EP_PWRUP			IMX_GPIO_NR(2, 8)	/* EPDC_PWRCTRL1 */
#define MX6SL_EP_INT			IMX_GPIO_NR(2, 9)	/* EPDC_PWRCTRL2 */
#define MX6SL_EP_PWRSTAT		IMX_GPIO_NR(2, 13)	/* EPDC_PWRSTAT */
#define MX6SL_EP_VCOM			IMX_GPIO_NR(2, 3)	/* EPDC_VCOM0 */

#define	MX6SL_SD2_CLK			IMX_GPIO_NR(5, 5)	/* SD2_CLK	 */		
#define	MX6SL_SD2_CMD			IMX_GPIO_NR(5, 4)	/* SD2_CMD	 */		
#define	MX6SL_SD2_DAT0			IMX_GPIO_NR(5, 1)	/* SD2_DAT0	 */		
#define	MX6SL_SD2_DAT1			IMX_GPIO_NR(4, 30)	/* SD2_DAT1	 */
#define	MX6SL_SD2_DAT2			IMX_GPIO_NR(5, 3) 	/* SD2_DAT2	 */
#define	MX6SL_SD2_DAT3			IMX_GPIO_NR(4, 28)	/* SD2_DAT3	 */

#define	MX6SL_SD3_CLK			IMX_GPIO_NR(5, 18)	/* SD3_CLK	 */		
#define	MX6SL_SD3_CMD			IMX_GPIO_NR(5, 21)	/* SD3_CMD	 */		
#define	MX6SL_SD3_DAT0			IMX_GPIO_NR(5, 19)	/* SD3_DAT0	 */		
#define	MX6SL_SD3_DAT1			IMX_GPIO_NR(5, 20)	/* SD3_DAT1	 */
#define	MX6SL_SD3_DAT2			IMX_GPIO_NR(5, 16) 	/* SD3_DAT2	 */
#define	MX6SL_SD3_DAT3			IMX_GPIO_NR(5, 17)	/* SD3_DAT3	 */

#define MX6SL_I2C2_SCL			IMX_GPIO_NR(3, 14)	/* I2C2_SCL */
#define MX6SL_I2C2_SDA			IMX_GPIO_NR(3, 15)	/* I2C2_SDA */
#define MX6SL_I2C3_SCL			IMX_GPIO_NR(3, 21)	/* I2C3_SCL */
#define MX6SL_I2C3_SDA			IMX_GPIO_NR(3, 22)	/* I2C3_SDA */

#define G_SENSOR_INT_4_18		IMX_GPIO_NR(4, 18)	/* FEC_RXD1 */

#define GPIO_ESD_3V3_ON		IMX_GPIO_NR(3, 29)	/* ROW2 */
#define GPIO_ISD_3V3_ON		IMX_GPIO_NR(3, 31)	/* ROW3 */
#define GPIO_IR_3V3_ON		IMX_GPIO_NR(4, 1)	/* ROW4 */
#define GPIO_EP_3V3_ON		IMX_GPIO_NR(4, 3)	/* ROW5 */

#define GPIO_KB_COL0			IMX_GPIO_NR(3, 24)	/* COL0 */
#define GPIO_KB_COL1			IMX_GPIO_NR(3, 26)	/* COL1 */

#define GPIO_KB_ROW0				IMX_GPIO_NR(3, 25)	/* ROW0 */
#define GPIO_KB_ROW1			IMX_GPIO_NR(3, 27)	/* ROW1 */


extern volatile unsigned gMX6SL_NTX_ACIN_PG;
extern volatile unsigned gMX6SL_NTX_CHG;
extern volatile unsigned gMX6SL_MSP_INT;
extern volatile unsigned gMX6SL_PWR_SW;
extern volatile unsigned gMX6SL_IR_TOUCH_INT;
extern volatile unsigned gMX6SL_IR_TOUCH_RST;
extern volatile unsigned gMX6SL_HALL_EN;
extern volatile unsigned gMX6SL_ON_LED;
extern volatile unsigned gMX6SL_CHG_LED;
extern volatile unsigned gMX6SL_ACT_LED;
extern volatile unsigned gMX6SL_WIFI_3V3;
extern volatile unsigned gMX6SL_WIFI_RST;
extern volatile unsigned gMX6SL_WIFI_INT;

static iomux_v3_cfg_t mx6sl_brd_ntx_kb_pads[] = {
	MX6SL_PAD_KEY_COL0__KPP_COL_0,
	MX6SL_PAD_KEY_COL1__KPP_COL_1,
	MX6SL_PAD_KEY_ROW0__KPP_ROW_0,
	MX6SL_PAD_KEY_ROW1__KPP_ROW_1,
};
static iomux_v3_cfg_t mx6sl_brd_ntx_kb_gpio_pads[] = {
	MX6SL_PAD_KEY_COL0__GPIO_3_24,
	MX6SL_PAD_KEY_COL1__GPIO_3_26,
	MX6SL_PAD_KEY_ROW0__GPIO_3_25,
	MX6SL_PAD_KEY_ROW1__GPIO_3_27,
};

static iomux_v3_cfg_t mx6sl_brd_ntx_sd4_pads[] = {
	MX6SL_PAD_FEC_MDIO__USDHC4_CLK,
	MX6SL_PAD_FEC_TX_CLK__USDHC4_CMD,
	MX6SL_PAD_FEC_RX_ER__USDHC4_DAT0,
	MX6SL_PAD_FEC_CRS_DV__USDHC4_DAT1,
	MX6SL_PAD_FEC_RXD1__USDHC4_DAT2,
	MX6SL_PAD_FEC_TXD0__USDHC4_DAT3,
	MX6SL_PAD_FEC_MDC__USDHC4_DAT4,
	MX6SL_PAD_FEC_RXD0__USDHC4_DAT5,
	MX6SL_PAD_FEC_TX_EN__USDHC4_DAT6,
	MX6SL_PAD_FEC_TXD1__USDHC4_DAT7,
	MX6SL_PAD_FEC_REF_CLK__GPIO_4_26_PULLHIGH,
};

static iomux_v3_cfg_t mx6sl_brd_ntx_sd4_gpio_pads[] = {
	MX6SL_PAD_FEC_MDIO__GPIO_4_20,		// PWR_GOOD
	MX6SL_PAD_FEC_TX_CLK__GPIO_4_21,	// CHG#
	MX6SL_PAD_FEC_RX_ER__GPIO_4_19,		// MSP_INT#
	MX6SL_PAD_FEC_CRS_DV__GPIO_4_25,	// PWR_SW
	MX6SL_PAD_FEC_RXD1__GPIO_4_18,		// GS_INT
	MX6SL_PAD_FEC_TXD0__GPIO_4_24,		// IR_TOUCH_INT#
	MX6SL_PAD_FEC_MDC__GPIO_4_23,		// HALL_EN
	MX6SL_PAD_FEC_RXD0__GPIO_4_17,		// IR_RST
	MX6SL_PAD_FEC_TX_EN__GPIO_4_22,		// ACT_LED
	MX6SL_PAD_FEC_TXD1__GPIO_4_16,		// CHG_LED
	MX6SL_PAD_FEC_REF_CLK__GPIO_4_26,	// ON_LED
};


static iomux_v3_cfg_t mx6sl_brd_ntx_sd1_pads[] = {
	MX6SL_PAD_SD1_CLK__USDHC1_CLK_50MHZ,
	MX6SL_PAD_SD1_CMD__USDHC1_CMD_50MHZ,
	MX6SL_PAD_SD1_DAT0__USDHC1_DAT0_50MHZ,
	MX6SL_PAD_SD1_DAT1__USDHC1_DAT1_50MHZ,
	MX6SL_PAD_SD1_DAT2__USDHC1_DAT2_50MHZ,
	MX6SL_PAD_SD1_DAT3__USDHC1_DAT3_50MHZ,
	MX6SL_PAD_SD1_DAT4__USDHC1_DAT4_50MHZ,
	MX6SL_PAD_SD1_DAT5__USDHC1_DAT5_50MHZ,
	MX6SL_PAD_SD1_DAT6__USDHC1_DAT6_50MHZ,
	MX6SL_PAD_SD1_DAT7__USDHC1_DAT7_50MHZ,
};

static iomux_v3_cfg_t mx6sl_brd_ntx_sd1_gpio_pads[] = {
	MX6SL_PAD_SD1_CLK__GPIO_5_15, // CHG# 
	MX6SL_PAD_SD1_CMD__GPIO_5_14, // ACIN_PG#
	MX6SL_PAD_SD1_DAT0__GPIO_5_11, // MSP_INT# 
	MX6SL_PAD_SD1_DAT1__GPIO_5_8, // PWR_SW#
	MX6SL_PAD_SD1_DAT2__GPIO_5_13, // TP85
	MX6SL_PAD_SD1_DAT3__GPIO_5_6, // IR_DataReady
	MX6SL_PAD_SD1_DAT4__GPIO_5_12, // TP86/Hall_EN#
	MX6SL_PAD_SD1_DAT5__GPIO_5_9, // V325_RST#
	MX6SL_PAD_SD1_DAT6__GPIO_5_7, // ACTION_LED_RH#
	MX6SL_PAD_SD1_DAT7__GPIO_5_10, // TP82/Charge_LED_RH#
};



static iomux_v3_cfg_t mx6sl_brd_ntx_pads[] = {
	MX6SL_PAD_REF_CLK_24M__I2C3_SCL,
	MX6SL_PAD_REF_CLK_32K__I2C3_SDA,
	
	MX6SL_PAD_SD2_DAT4__GPIO_5_2,		// EXT_SD_CD
	MX6SL_PAD_SD2_DAT7__GPIO_5_0,		// WIFI_3V3_ON
	MX6SL_PAD_SD2_RST__GPIO_4_27,		// WIFI_RST
	//MX6SL_PAD_SD2_DAT6__GPIO_4_29_OUTPUT,		// WIFI_INT
	
	MX6SL_PAD_KEY_COL5__GPIO_4_2,		// HW_ID0
	MX6SL_PAD_KEY_COL6__GPIO_4_4,		// HW_ID1
	MX6SL_PAD_KEY_COL7__GPIO_4_6,		// HW_ID2
	MX6SL_PAD_KEY_ROW6__GPIO_4_5,		// HW_ID3
	MX6SL_PAD_KEY_ROW7__GPIO_4_7,		// HW_ID4

	MX6SL_PAD_KEY_ROW2__GPIO_3_29,  // ESD_3V3_ON(Q22)/BS_INT(Q12)/NA
	MX6SL_PAD_KEY_ROW3__GPIO_3_31,  // ISD_3V3_ON(Q22)/BS_3V3(Q12/NA
	MX6SL_PAD_KEY_ROW4__GPIO_4_1, // IR_3V3_ON(Q22)/BS_UP(Q12)/NA
	MX6SL_PAD_KEY_ROW5__GPIO_4_3, // EP_3V3_ON(Q22)/BS_DN(Q12)/NA
	
	MX6SL_PAD_KEY_COL2__GPIO_3_28, // BS_Left (Q12)/NA
	MX6SL_PAD_KEY_COL3__GPIO_3_30, // BS_Right (Q12)/NA
	MX6SL_PAD_KEY_COL4__GPIO_4_0, // BS_Click (Q12)/NA

#if 0	
	MX6SL_PAD_EPDC_PWRCTRL0__GPIO_2_7,	// EP_PWRCTRL0 (WAKEUP#)
	MX6SL_PAD_EPDC_PWRCTRL1__GPIO_2_8,	// EP_PWRCTRL1 (PWRUP)
	MX6SL_PAD_EPDC_PWRCTRL2__GPIO_2_9,	// EP_INT
	MX6SL_PAD_EPDC_PWRWAKEUP__GPIO_2_14,// PWRALL
	MX6SL_PAD_EPDC_PWRSTAT__GPIO_2_13,	// EP_PWRSTAT (PWR_GOOD)
	MX6SL_PAD_EPDC_VCOM0__GPIO_2_3,		// VCOM_CTRL
#endif

	MX6SL_PAD_EPDC_PWRCTRL3__GPIO_2_10,	// FL_EN
	MX6SL_PAD_EPDC_SDCE2__GPIO_1_29,	// FL_R_EN
	
	MX6SL_PAD_LCD_DAT0__GPIO_2_20,
	MX6SL_PAD_LCD_DAT1__GPIO_2_21,
	MX6SL_PAD_LCD_DAT2__GPIO_2_22,
	MX6SL_PAD_LCD_DAT3__GPIO_2_23,
	MX6SL_PAD_LCD_DAT4__GPIO_2_24,
	MX6SL_PAD_LCD_DAT5__GPIO_2_25,
	MX6SL_PAD_LCD_DAT6__GPIO_2_26,
	MX6SL_PAD_LCD_DAT7__GPIO_2_27,
	MX6SL_PAD_LCD_DAT8__GPIO_2_28,
	MX6SL_PAD_LCD_DAT9__GPIO_2_29,
	MX6SL_PAD_LCD_DAT10__GPIO_2_30,
	MX6SL_PAD_LCD_DAT11__GPIO_2_31,
	MX6SL_PAD_LCD_DAT12__GPIO_3_0,
	MX6SL_PAD_LCD_DAT13__GPIO_3_1,
	MX6SL_PAD_LCD_DAT14__GPIO_3_2,
	MX6SL_PAD_LCD_DAT15__GPIO_3_3,
	MX6SL_PAD_LCD_DAT16__GPIO_3_4,
	MX6SL_PAD_LCD_DAT17__GPIO_3_5,
	MX6SL_PAD_LCD_DAT18__GPIO_3_6,
	MX6SL_PAD_LCD_DAT19__GPIO_3_7,
	MX6SL_PAD_LCD_DAT20__GPIO_3_8,
	MX6SL_PAD_LCD_DAT21__GPIO_3_9,
	MX6SL_PAD_LCD_DAT22__GPIO_3_10,
	MX6SL_PAD_LCD_DAT23__GPIO_3_11,
	
	MX6SL_PAD_LCD_CLK__GPIO_2_15,
	MX6SL_PAD_LCD_ENABLE__GPIO_2_16,
	MX6SL_PAD_LCD_HSYNC__GPIO_2_17,
	MX6SL_PAD_LCD_VSYNC__GPIO_2_18,
	MX6SL_PAD_LCD_RESET__GPIO_2_19,

	MX6SL_PAD_ECSPI1_SCLK__GPIO_4_8,
	MX6SL_PAD_ECSPI1_MOSI__GPIO_4_9,
	MX6SL_PAD_ECSPI1_MISO__GPIO_4_10,
	MX6SL_PAD_ECSPI1_SS0__GPIO_4_11,
	MX6SL_PAD_ECSPI2_SCLK__GPIO_4_12,
	MX6SL_PAD_ECSPI2_MOSI__GPIO_4_13,
	MX6SL_PAD_ECSPI2_MISO__GPIO_4_14,
	MX6SL_PAD_ECSPI2_SS0__GPIO_4_15,
	MX6SL_PAD_AUD_RXFS__GPIO_1_0,
	MX6SL_PAD_AUD_RXC__GPIO_1_1,
	MX6SL_PAD_AUD_RXD__GPIO_1_2,
	MX6SL_PAD_AUD_TXC__GPIO_1_3,
	MX6SL_PAD_AUD_TXFS__GPIO_1_4,
	MX6SL_PAD_AUD_TXD__GPIO_1_5,
	MX6SL_PAD_AUD_MCLK__GPIO_1_6,	
	MX6SL_PAD_HSIC_DAT__GPIO_3_19,
	MX6SL_PAD_HSIC_STROBE__GPIO_3_20,
	MX6SL_PAD_WDOG_B__GPIO_3_18,
};

static iomux_v3_cfg_t mx6sl_ntx_sd2_gpio_pads[] = {
	MX6SL_PAD_SD2_CLK__GPIO_5_5,
	MX6SL_PAD_SD2_CMD__GPIO_5_4,
	MX6SL_PAD_SD2_DAT0__GPIO_5_1,
	MX6SL_PAD_SD2_DAT1__GPIO_4_30,
	MX6SL_PAD_SD2_DAT2__GPIO_5_3,
	MX6SL_PAD_SD2_DAT3__GPIO_4_28,
};

static iomux_v3_cfg_t mx6sl_ntx_sd2_wifi_pads[] = {
	MX6SL_PAD_SD2_CLK__USDHC2_CLK_50MHZ,
	MX6SL_PAD_SD2_CMD__USDHC2_CMD_50MHZ,
	MX6SL_PAD_SD2_DAT0__USDHC2_DAT0_50MHZ,
	MX6SL_PAD_SD2_DAT1__USDHC2_DAT1_50MHZ,
	MX6SL_PAD_SD2_DAT2__USDHC2_DAT2_50MHZ,
	MX6SL_PAD_SD2_DAT3__USDHC2_DAT3_50MHZ,
};

static iomux_v3_cfg_t mx6sl_ntx_sd3_gpio_pads[] = {
	MX6SL_PAD_SD3_CLK__GPIO_5_18,
	MX6SL_PAD_SD3_CMD__GPIO_5_21,
	MX6SL_PAD_SD3_DAT0__GPIO_5_19,
	MX6SL_PAD_SD3_DAT1__GPIO_5_20,
	MX6SL_PAD_SD3_DAT2__GPIO_5_16,
	MX6SL_PAD_SD3_DAT3__GPIO_5_17,
};

static iomux_v3_cfg_t mx6sl_ntx_sd3_wifi_pads[] = {
	MX6SL_PAD_SD3_CLK__USDHC3_CLK_50MHZ,
	MX6SL_PAD_SD3_CMD__USDHC3_CMD_50MHZ,
	MX6SL_PAD_SD3_DAT0__USDHC3_DAT0_50MHZ,
	MX6SL_PAD_SD3_DAT1__USDHC3_DAT1_50MHZ,
	MX6SL_PAD_SD3_DAT2__USDHC3_DAT2_50MHZ,
	MX6SL_PAD_SD3_DAT3__USDHC3_DAT3_50MHZ,
};


static iomux_v3_cfg_t mx6sl_ntx_q22_wifictrl_pads[] = {
	MX6SL_PAD_SD2_DAT6__GPIO_4_29_OUTPUT, // WiFi_3V3_ON#
	MX6SL_PAD_SD2_DAT5__GPIO_4_31_INPUT, // WiFi_INT#
};

static iomux_v3_cfg_t mx6sl_ntx_q12_wifictrl_pads[] = {
	MX6SL_PAD_SD2_DAT6__GPIO_4_29, // WiFi_INT#
	MX6SL_PAD_SD2_DAT5__GPIO_4_31, //NC
};

#endif
