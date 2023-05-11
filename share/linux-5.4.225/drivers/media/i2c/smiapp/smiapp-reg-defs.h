/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * drivers/media/i2c/smiapp/smiapp-reg-defs.h
 *
 * Generic driver for SMIA/SMIA++ compliant camera modules
 *
 * Copyright (C) 2011--2012 Nokia Corporation
 * Contact: Sakari Ailus <sakari.ailus@iki.fi>
 */
#define SMIAPP_REG_MK_U8(r) ((SMIAPP_REG_8BIT << 16) | (r))
#define SMIAPP_REG_MK_U16(r) ((SMIAPP_REG_16BIT << 16) | (r))
#define SMIAPP_REG_MK_U32(r) ((SMIAPP_REG_32BIT << 16) | (r))

#define SMIAPP_REG_MK_F32(r) (SMIAPP_REG_FLAG_FLOAT | (SMIAPP_REG_32BIT << 16) | (r))

#define SMIAPP_REG_U16_MODEL_ID					SMIAPP_REG_MK_U16(0x0000)
#define SMIAPP_REG_U8_REVISION_NUMBER_MAJOR			SMIAPP_REG_MK_U8(0x0002)
#define SMIAPP_REG_U8_MANUFACTURER_ID				SMIAPP_REG_MK_U8(0x0003)
#define SMIAPP_REG_U8_SMIA_VERSION				SMIAPP_REG_MK_U8(0x0004)
#define SMIAPP_REG_U8_FRAME_COUNT				SMIAPP_REG_MK_U8(0x0005)
#define SMIAPP_REG_U8_PIXEL_ORDER				SMIAPP_REG_MK_U8(0x0006)
#define SMIAPP_REG_U16_DATA_PEDESTAL				SMIAPP_REG_MK_U16(0x0008)
#define SMIAPP_REG_U8_PIXEL_DEPTH				SMIAPP_REG_MK_U8(0x000c)
#define SMIAPP_REG_U8_REVISION_NUMBER_MINOR			SMIAPP_REG_MK_U8(0x0010)
#define SMIAPP_REG_U8_SMIAPP_VERSION				SMIAPP_REG_MK_U8(0x0011)
#define SMIAPP_REG_U8_MODULE_DATE_YEAR				SMIAPP_REG_MK_U8(0x0012)
#define SMIAPP_REG_U8_MODULE_DATE_MONTH				SMIAPP_REG_MK_U8(0x0013)
#define SMIAPP_REG_U8_MODULE_DATE_DAY				SMIAPP_REG_MK_U8(0x0014)
#define SMIAPP_REG_U8_MODULE_DATE_PHASE				SMIAPP_REG_MK_U8(0x0015)
#define SMIAPP_REG_U16_SENSOR_MODEL_ID				SMIAPP_REG_MK_U16(0x0016)
#define SMIAPP_REG_U8_SENSOR_REVISION_NUMBER			SMIAPP_REG_MK_U8(0x0018)
#define SMIAPP_REG_U8_SENSOR_MANUFACTURER_ID			SMIAPP_REG_MK_U8(0x0019)
#define SMIAPP_REG_U8_SENSOR_FIRMWARE_VERSION			SMIAPP_REG_MK_U8(0x001a)
#define SMIAPP_REG_U32_SERIAL_NUMBER				SMIAPP_REG_MK_U32(0x001c)
#define SMIAPP_REG_U8_FRAME_FORMAT_MODEL_TYPE			SMIAPP_REG_MK_U8(0x0040)
#define SMIAPP_REG_U8_FRAME_FORMAT_MODEL_SUBTYPE		SMIAPP_REG_MK_U8(0x0041)
#define SMIAPP_REG_U16_FRAME_FORMAT_DESCRIPTOR_2(n)		SMIAPP_REG_MK_U16(0x0042 + ((n) << 1)) /* 0 <= n <= 14 */
#define SMIAPP_REG_U32_FRAME_FORMAT_DESCRIPTOR_4(n)		SMIAPP_REG_MK_U32(0x0060 + ((n) << 2)) /* 0 <= n <= 7 */
#define SMIAPP_REG_U16_ANALOGUE_GAIN_CAPABILITY			SMIAPP_REG_MK_U16(0x0080)
#define SMIAPP_REG_U16_ANALOGUE_GAIN_CODE_MIN			SMIAPP_REG_MK_U16(0x0084)
#define SMIAPP_REG_U16_ANALOGUE_GAIN_CODE_MAX			SMIAPP_REG_MK_U16(0x0086)
#define SMIAPP_REG_U16_ANALOGUE_GAIN_CODE_STEP			SMIAPP_REG_MK_U16(0x0088)
#define SMIAPP_REG_U16_ANALOGUE_GAIN_TYPE			SMIAPP_REG_MK_U16(0x008a)
#define SMIAPP_REG_U16_ANALOGUE_GAIN_M0				SMIAPP_REG_MK_U16(0x008c)
#define SMIAPP_REG_U16_ANALOGUE_GAIN_C0				SMIAPP_REG_MK_U16(0x008e)
#define SMIAPP_REG_U16_ANALOGUE_GAIN_M1				SMIAPP_REG_MK_U16(0x0090)
#define SMIAPP_REG_U16_ANALOGUE_GAIN_C1				SMIAPP_REG_MK_U16(0x0092)
#define SMIAPP_REG_U8_DATA_FORMAT_MODEL_TYPE			SMIAPP_REG_MK_U8(0x00c0)
#define SMIAPP_REG_U8_DATA_FORMAT_MODEL_SUBTYPE			SMIAPP_REG_MK_U8(0x00c1)
#define SMIAPP_REG_U16_DATA_FORMAT_DESCRIPTOR(n)		SMIAPP_REG_MK_U16(0x00c2 + ((n) << 1))
#define SMIAPP_REG_U8_MODE_SELECT				SMIAPP_REG_MK_U8(0x0100)
#define SMIAPP_REG_U8_IMAGE_ORIENTATION				SMIAPP_REG_MK_U8(0x0101)
#define SMIAPP_REG_U8_SOFTWARE_RESET				SMIAPP_REG_MK_U8(0x0103)
#define SMIAPP_REG_U8_GROUPED_PARAMETER_HOLD			SMIAPP_REG_MK_U8(0x0104)
#define SMIAPP_REG_U8_MASK_CORRUPTED_FRAMES			SMIAPP_REG_MK_U8(0x0105)
#define SMIAPP_REG_U8_FAST_STANDBY_CTRL				SMIAPP_REG_MK_U8(0x0106)
#define SMIAPP_REG_U8_CCI_ADDRESS_CONTROL			SMIAPP_REG_MK_U8(0x0107)
#define SMIAPP_REG_U8_2ND_CCI_IF_CONTROL			SMIAPP_REG_MK_U8(0x0108)
#define SMIAPP_REG_U8_2ND_CCI_ADDRESS_CONTROL			SMIAPP_REG_MK_U8(0x0109)
#define SMIAPP_REG_U8_CSI_CHANNEL_IDENTIFIER			SMIAPP_REG_MK_U8(0x0110)
#define SMIAPP_REG_U8_CSI_SIGNALLING_MODE			SMIAPP_REG_MK_U8(0x0111)
#define SMIAPP_REG_U16_CSI_DATA_FORMAT				SMIAPP_REG_MK_U16(0x0112)
#define SMIAPP_REG_U8_CSI_LANE_MODE				SMIAPP_REG_MK_U8(0x0114)
#define SMIAPP_REG_U8_CSI2_10_TO_8_DT				SMIAPP_REG_MK_U8(0x0115)
#define SMIAPP_REG_U8_CSI2_10_TO_7_DT				SMIAPP_REG_MK_U8(0x0116)
#define SMIAPP_REG_U8_CSI2_10_TO_6_DT				SMIAPP_REG_MK_U8(0x0117)
#define SMIAPP_REG_U8_CSI2_12_TO_8_DT				SMIAPP_REG_MK_U8(0x0118)
#define SMIAPP_REG_U8_CSI2_12_TO_7_DT				SMIAPP_REG_MK_U8(0x0119)
#define SMIAPP_REG_U8_CSI2_12_TO_6_DT				SMIAPP_REG_MK_U8(0x011a)
#define SMIAPP_REG_U8_CSI2_14_TO_10_DT				SMIAPP_REG_MK_U8(0x011b)
#define SMIAPP_REG_U8_CSI2_14_TO_8_DT				SMIAPP_REG_MK_U8(0x011c)
#define SMIAPP_REG_U8_CSI2_16_TO_10_DT				SMIAPP_REG_MK_U8(0x011d)
#define SMIAPP_REG_U8_CSI2_16_TO_8_DT				SMIAPP_REG_MK_U8(0x011e)
#define SMIAPP_REG_U8_GAIN_MODE					SMIAPP_REG_MK_U8(0x0120)
#define SMIAPP_REG_U16_VANA_VOLTAGE				SMIAPP_REG_MK_U16(0x0130)
#define SMIAPP_REG_U16_VDIG_VOLTAGE				SMIAPP_REG_MK_U16(0x0132)
#define SMIAPP_REG_U16_VIO_VOLTAGE				SMIAPP_REG_MK_U16(0x0134)
#define SMIAPP_REG_U16_EXTCLK_FREQUENCY_MHZ			SMIAPP_REG_MK_U16(0x0136)
#define SMIAPP_REG_U8_TEMP_SENSOR_CONTROL			SMIAPP_REG_MK_U8(0x0138)
#define SMIAPP_REG_U8_TEMP_SENSOR_MODE				SMIAPP_REG_MK_U8(0x0139)
#define SMIAPP_REG_U8_TEMP_SENSOR_OUTPUT			SMIAPP_REG_MK_U8(0x013a)
#define SMIAPP_REG_U16_FINE_INTEGRATION_TIME			SMIAPP_REG_MK_U16(0x0200)
#define SMIAPP_REG_U16_COARSE_INTEGRATION_TIME			SMIAPP_REG_MK_U16(0x0202)
#define SMIAPP_REG_U16_ANALOGUE_GAIN_CODE_GLOBAL		SMIAPP_REG_MK_U16(0x0204)
#define SMIAPP_REG_U16_ANALOGUE_GAIN_CODE_GREENR		SMIAPP_REG_MK_U16(0x0206)
#define SMIAPP_REG_U16_ANALOGUE_GAIN_CODE_RED			SMIAPP_REG_MK_U16(0x0208)
#define SMIAPP_REG_U16_ANALOGUE_GAIN_CODE_BLUE			SMIAPP_REG_MK_U16(0x020a)
#define SMIAPP_REG_U16_ANALOGUE_GAIN_CODE_GREENB		SMIAPP_REG_MK_U16(0x020c)
#define SMIAPP_REG_U16_DIGITAL_GAIN_GREENR			SMIAPP_REG_MK_U16(0x020e)
#define SMIAPP_REG_U16_DIGITAL_GAIN_RED				SMIAPP_REG_MK_U16(0x0210)
#define SMIAPP_REG_U16_DIGITAL_GAIN_BLUE			SMIAPP_REG_MK_U16(0x0212)
#define SMIAPP_REG_U16_DIGITAL_GAIN_GREENB			SMIAPP_REG_MK_U16(0x0214)
#define SMIAPP_REG_U16_VT_PIX_CLK_DIV				SMIAPP_REG_MK_U16(0x0300)
#define SMIAPP_REG_U16_VT_SYS_CLK_DIV				SMIAPP_REG_MK_U16(0x0302)
#define SMIAPP_REG_U16_PRE_PLL_CLK_DIV				SMIAPP_REG_MK_U16(0x0304)
#define SMIAPP_REG_U16_PLL_MULTIPLIER				SMIAPP_REG_MK_U16(0x0306)
#define SMIAPP_REG_U16_OP_PIX_CLK_DIV				SMIAPP_REG_MK_U16(0x0308)
#define SMIAPP_REG_U16_OP_SYS_CLK_DIV				SMIAPP_REG_MK_U16(0x030a)
#define SMIAPP_REG_U16_FRAME_LENGTH_LINES			SMIAPP_REG_MK_U16(0x0340)
#define SMIAPP_REG_U16_LINE_LENGTH_PCK				SMIAPP_REG_MK_U16(0x0342)
#define SMIAPP_REG_U16_X_ADDR_START				SMIAPP_REG_MK_U16(0x0344)
#define SMIAPP_REG_U16_Y_ADDR_START				SMIAPP_REG_MK_U16(0x0346)
#define SMIAPP_REG_U16_X_ADDR_END				SMIAPP_REG_MK_U16(0x0348)
#define SMIAPP_REG_U16_Y_ADDR_END				SMIAPP_REG_MK_U16(0x034a)
#define SMIAPP_REG_U16_X_OUTPUT_SIZE				SMIAPP_REG_MK_U16(0x034c)
#define SMIAPP_REG_U16_Y_OUTPUT_SIZE				SMIAPP_REG_MK_U16(0x034e)
#define SMIAPP_REG_U16_X_EVEN_INC				SMIAPP_REG_MK_U16(0x0380)
#define SMIAPP_REG_U16_X_ODD_INC				SMIAPP_REG_MK_U16(0x0382)
#define SMIAPP_REG_U16_Y_EVEN_INC				SMIAPP_REG_MK_U16(0x0384)
#define SMIAPP_REG_U16_Y_ODD_INC				SMIAPP_REG_MK_U16(0x0386)
#define SMIAPP_REG_U16_SCALING_MODE				SMIAPP_REG_MK_U16(0x0400)
#define SMIAPP_REG_U16_SPATIAL_SAMPLING				SMIAPP_REG_MK_U16(0x0402)
#define SMIAPP_REG_U16_SCALE_M					SMIAPP_REG_MK_U16(0x0404)
#define SMIAPP_REG_U16_SCALE_N					SMIAPP_REG_MK_U16(0x0406)
#define SMIAPP_REG_U16_DIGITAL_CROP_X_OFFSET			SMIAPP_REG_MK_U16(0x0408)
#define SMIAPP_REG_U16_DIGITAL_CROP_Y_OFFSET			SMIAPP_REG_MK_U16(0x040a)
#define SMIAPP_REG_U16_DIGITAL_CROP_IMAGE_WIDTH			SMIAPP_REG_MK_U16(0x040c)
#define SMIAPP_REG_U16_DIGITAL_CROP_IMAGE_HEIGHT		SMIAPP_REG_MK_U16(0x040e)
#define SMIAPP_REG_U16_COMPRESSION_MODE				SMIAPP_REG_MK_U16(0x0500)
#define SMIAPP_REG_U16_TEST_PATTERN_MODE			SMIAPP_REG_MK_U16(0x0600)
#define SMIAPP_REG_U16_TEST_DATA_RED				SMIAPP_REG_MK_U16(0x0602)
#define SMIAPP_REG_U16_TEST_DATA_GREENR				SMIAPP_REG_MK_U16(0x0604)
#define SMIAPP_REG_U16_TEST_DATA_BLUE				SMIAPP_REG_MK_U16(0x0606)
#define SMIAPP_REG_U16_TEST_DATA_GREENB				SMIAPP_REG_MK_U16(0x0608)
#define SMIAPP_REG_U16_HORIZONTAL_CURSOR_WIDTH			SMIAPP_REG_MK_U16(0x060a)
#define SMIAPP_REG_U16_HORIZONTAL_CURSOR_POSITION		SMIAPP_REG_MK_U16(0x060c)
#define SMIAPP_REG_U16_VERTICAL_CURSOR_WIDTH			SMIAPP_REG_MK_U16(0x060e)
#define SMIAPP_REG_U16_VERTICAL_CURSOR_POSITION			SMIAPP_REG_MK_U16(0x0610)
#define SMIAPP_REG_U16_FIFO_WATER_MARK_PIXELS			SMIAPP_REG_MK_U16(0x0700)
#define SMIAPP_REG_U8_TCLK_POST					SMIAPP_REG_MK_U8(0x0800)
#define SMIAPP_REG_U8_THS_PREPARE				SMIAPP_REG_MK_U8(0x0801)
#define SMIAPP_REG_U8_THS_ZERO_MIN				SMIAPP_REG_MK_U8(0x0802)
#define SMIAPP_REG_U8_THS_TRAIL					SMIAPP_REG_MK_U8(0x0803)
#define SMIAPP_REG_U8_TCLK_TRAIL_MIN				SMIAPP_REG_MK_U8(0x0804)
#define SMIAPP_REG_U8_TCLK_PREPARE				SMIAPP_REG_MK_U8(0x0805)
#define SMIAPP_REG_U8_TCLK_ZERO					SMIAPP_REG_MK_U8(0x0806)
#define SMIAPP_REG_U8_TLPX					SMIAPP_REG_MK_U8(0x0807)
#define SMIAPP_REG_U8_DPHY_CTRL					SMIAPP_REG_MK_U8(0x0808)
#define SMIAPP_REG_U32_REQUESTED_LINK_BIT_RATE_MBPS		SMIAPP_REG_MK_U32(0x0820)
#define SMIAPP_REG_U8_BINNING_MODE				SMIAPP_REG_MK_U8(0x0900)
#define SMIAPP_REG_U8_BINNING_TYPE				SMIAPP_REG_MK_U8(0x0901)
#define SMIAPP_REG_U8_BINNING_WEIGHTING				SMIAPP_REG_MK_U8(0x0902)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_CTRL			SMIAPP_REG_MK_U8(0x0a00)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_STATUS			SMIAPP_REG_MK_U8(0x0a01)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_PAGE_SELECT		SMIAPP_REG_MK_U8(0x0a02)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_0			SMIAPP_REG_MK_U8(0x0a04)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_1			SMIAPP_REG_MK_U8(0x0a05)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_2			SMIAPP_REG_MK_U8(0x0a06)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_3			SMIAPP_REG_MK_U8(0x0a07)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_4			SMIAPP_REG_MK_U8(0x0a08)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_5			SMIAPP_REG_MK_U8(0x0a09)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_12		SMIAPP_REG_MK_U8(0x0a10)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_13		SMIAPP_REG_MK_U8(0x0a11)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_14		SMIAPP_REG_MK_U8(0x0a12)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_15		SMIAPP_REG_MK_U8(0x0a13)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_16		SMIAPP_REG_MK_U8(0x0a14)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_17		SMIAPP_REG_MK_U8(0x0a15)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_18		SMIAPP_REG_MK_U8(0x0a16)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_19		SMIAPP_REG_MK_U8(0x0a17)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_20		SMIAPP_REG_MK_U8(0x0a18)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_21		SMIAPP_REG_MK_U8(0x0a19)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_22		SMIAPP_REG_MK_U8(0x0a1a)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_23		SMIAPP_REG_MK_U8(0x0a1b)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_24		SMIAPP_REG_MK_U8(0x0a1c)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_25		SMIAPP_REG_MK_U8(0x0a1d)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_26		SMIAPP_REG_MK_U8(0x0a1e)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_27		SMIAPP_REG_MK_U8(0x0a1f)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_28		SMIAPP_REG_MK_U8(0x0a20)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_29		SMIAPP_REG_MK_U8(0x0a21)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_30		SMIAPP_REG_MK_U8(0x0a22)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_31		SMIAPP_REG_MK_U8(0x0a23)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_32		SMIAPP_REG_MK_U8(0x0a24)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_33		SMIAPP_REG_MK_U8(0x0a25)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_34		SMIAPP_REG_MK_U8(0x0a26)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_35		SMIAPP_REG_MK_U8(0x0a27)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_36		SMIAPP_REG_MK_U8(0x0a28)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_37		SMIAPP_REG_MK_U8(0x0a29)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_38		SMIAPP_REG_MK_U8(0x0a2a)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_39		SMIAPP_REG_MK_U8(0x0a2b)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_40		SMIAPP_REG_MK_U8(0x0a2c)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_41		SMIAPP_REG_MK_U8(0x0a2d)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_42		SMIAPP_REG_MK_U8(0x0a2e)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_43		SMIAPP_REG_MK_U8(0x0a2f)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_44		SMIAPP_REG_MK_U8(0x0a30)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_45		SMIAPP_REG_MK_U8(0x0a31)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_46		SMIAPP_REG_MK_U8(0x0a32)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_47		SMIAPP_REG_MK_U8(0x0a33)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_48		SMIAPP_REG_MK_U8(0x0a34)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_49		SMIAPP_REG_MK_U8(0x0a35)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_50		SMIAPP_REG_MK_U8(0x0a36)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_51		SMIAPP_REG_MK_U8(0x0a37)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_52		SMIAPP_REG_MK_U8(0x0a38)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_53		SMIAPP_REG_MK_U8(0x0a39)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_54		SMIAPP_REG_MK_U8(0x0a3a)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_55		SMIAPP_REG_MK_U8(0x0a3b)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_56		SMIAPP_REG_MK_U8(0x0a3c)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_57		SMIAPP_REG_MK_U8(0x0a3d)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_58		SMIAPP_REG_MK_U8(0x0a3e)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_59		SMIAPP_REG_MK_U8(0x0a3f)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_60		SMIAPP_REG_MK_U8(0x0a40)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_61		SMIAPP_REG_MK_U8(0x0a41)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_62		SMIAPP_REG_MK_U8(0x0a42)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_1_DATA_63		SMIAPP_REG_MK_U8(0x0a43)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_CTRL			SMIAPP_REG_MK_U8(0x0a44)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_STATUS			SMIAPP_REG_MK_U8(0x0a45)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_PAGE_SELECT		SMIAPP_REG_MK_U8(0x0a46)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_0			SMIAPP_REG_MK_U8(0x0a48)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_1			SMIAPP_REG_MK_U8(0x0a49)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_2			SMIAPP_REG_MK_U8(0x0a4a)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_3			SMIAPP_REG_MK_U8(0x0a4b)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_4			SMIAPP_REG_MK_U8(0x0a4c)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_5			SMIAPP_REG_MK_U8(0x0a4d)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_6			SMIAPP_REG_MK_U8(0x0a4e)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_7			SMIAPP_REG_MK_U8(0x0a4f)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_8			SMIAPP_REG_MK_U8(0x0a50)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_9			SMIAPP_REG_MK_U8(0x0a51)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_10		SMIAPP_REG_MK_U8(0x0a52)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_11		SMIAPP_REG_MK_U8(0x0a53)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_12		SMIAPP_REG_MK_U8(0x0a54)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_13		SMIAPP_REG_MK_U8(0x0a55)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_14		SMIAPP_REG_MK_U8(0x0a56)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_15		SMIAPP_REG_MK_U8(0x0a57)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_16		SMIAPP_REG_MK_U8(0x0a58)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_17		SMIAPP_REG_MK_U8(0x0a59)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_18		SMIAPP_REG_MK_U8(0x0a5a)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_19		SMIAPP_REG_MK_U8(0x0a5b)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_20		SMIAPP_REG_MK_U8(0x0a5c)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_21		SMIAPP_REG_MK_U8(0x0a5d)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_22		SMIAPP_REG_MK_U8(0x0a5e)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_23		SMIAPP_REG_MK_U8(0x0a5f)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_24		SMIAPP_REG_MK_U8(0x0a60)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_25		SMIAPP_REG_MK_U8(0x0a61)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_26		SMIAPP_REG_MK_U8(0x0a62)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_27		SMIAPP_REG_MK_U8(0x0a63)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_28		SMIAPP_REG_MK_U8(0x0a64)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_29		SMIAPP_REG_MK_U8(0x0a65)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_30		SMIAPP_REG_MK_U8(0x0a66)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_31		SMIAPP_REG_MK_U8(0x0a67)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_32		SMIAPP_REG_MK_U8(0x0a68)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_33		SMIAPP_REG_MK_U8(0x0a69)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_34		SMIAPP_REG_MK_U8(0x0a6a)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_35		SMIAPP_REG_MK_U8(0x0a6b)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_36		SMIAPP_REG_MK_U8(0x0a6c)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_37		SMIAPP_REG_MK_U8(0x0a6d)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_38		SMIAPP_REG_MK_U8(0x0a6e)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_39		SMIAPP_REG_MK_U8(0x0a6f)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_40		SMIAPP_REG_MK_U8(0x0a70)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_41		SMIAPP_REG_MK_U8(0x0a71)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_42		SMIAPP_REG_MK_U8(0x0a72)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_43		SMIAPP_REG_MK_U8(0x0a73)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_44		SMIAPP_REG_MK_U8(0x0a74)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_45		SMIAPP_REG_MK_U8(0x0a75)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_46		SMIAPP_REG_MK_U8(0x0a76)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_47		SMIAPP_REG_MK_U8(0x0a77)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_48		SMIAPP_REG_MK_U8(0x0a78)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_49		SMIAPP_REG_MK_U8(0x0a79)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_50		SMIAPP_REG_MK_U8(0x0a7a)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_51		SMIAPP_REG_MK_U8(0x0a7b)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_52		SMIAPP_REG_MK_U8(0x0a7c)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_53		SMIAPP_REG_MK_U8(0x0a7d)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_54		SMIAPP_REG_MK_U8(0x0a7e)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_55		SMIAPP_REG_MK_U8(0x0a7f)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_56		SMIAPP_REG_MK_U8(0x0a80)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_57		SMIAPP_REG_MK_U8(0x0a81)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_58		SMIAPP_REG_MK_U8(0x0a82)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_59		SMIAPP_REG_MK_U8(0x0a83)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_60		SMIAPP_REG_MK_U8(0x0a84)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_61		SMIAPP_REG_MK_U8(0x0a85)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_62		SMIAPP_REG_MK_U8(0x0a86)
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_2_DATA_63		SMIAPP_REG_MK_U8(0x0a87)
#define SMIAPP_REG_U8_SHADING_CORRECTION_ENABLE			SMIAPP_REG_MK_U8(0x0b00)
#define SMIAPP_REG_U8_LUMINANCE_CORRECTION_LEVEL		SMIAPP_REG_MK_U8(0x0b01)
#define SMIAPP_REG_U8_GREEN_IMBALANCE_FILTER_ENABLE		SMIAPP_REG_MK_U8(0x0b02)
#define SMIAPP_REG_U8_GREEN_IMBALANCE_FILTER_WEIGHT		SMIAPP_REG_MK_U8(0x0b03)
#define SMIAPP_REG_U8_BLACK_LEVEL_CORRECTION_ENABLE		SMIAPP_REG_MK_U8(0x0b04)
#define SMIAPP_REG_U8_MAPPED_COUPLET_CORRECT_ENABLE		SMIAPP_REG_MK_U8(0x0b05)
#define SMIAPP_REG_U8_SINGLE_DEFECT_CORRECT_ENABLE		SMIAPP_REG_MK_U8(0x0b06)
#define SMIAPP_REG_U8_SINGLE_DEFECT_CORRECT_WEIGHT		SMIAPP_REG_MK_U8(0x0b07)
#define SMIAPP_REG_U8_DYNAMIC_COUPLET_CORRECT_ENABLE		SMIAPP_REG_MK_U8(0x0b08)
#define SMIAPP_REG_U8_DYNAMIC_COUPLET_CORRECT_WEIGHT		SMIAPP_REG_MK_U8(0x0b09)
#define SMIAPP_REG_U8_COMBINED_DEFECT_CORRECT_ENABLE		SMIAPP_REG_MK_U8(0x0b0a)
#define SMIAPP_REG_U8_COMBINED_DEFECT_CORRECT_WEIGHT		SMIAPP_REG_MK_U8(0x0b0b)
#define SMIAPP_REG_U8_MODULE_SPECIFIC_CORRECTION_ENABLE		SMIAPP_REG_MK_U8(0x0b0c)
#define SMIAPP_REG_U8_MODULE_SPECIFIC_CORRECTION_WEIGHT		SMIAPP_REG_MK_U8(0x0b0d)
#define SMIAPP_REG_U8_MAPPED_LINE_DEFECT_CORRECT_ENABLE		SMIAPP_REG_MK_U8(0x0b0e)
#define SMIAPP_REG_U8_MAPPED_LINE_DEFECT_CORRECT_ADJUST		SMIAPP_REG_MK_U8(0x0b0f)
#define SMIAPP_REG_U8_MAPPED_COUPLET_CORRECT_ADJUST		SMIAPP_REG_MK_U8(0x0b10)
#define SMIAPP_REG_U8_MAPPED_TRIPLET_DEFECT_CORRECT_ENABLE	SMIAPP_REG_MK_U8(0x0b11)
#define SMIAPP_REG_U8_MAPPED_TRIPLET_DEFECT_CORRECT_ADJUST	SMIAPP_REG_MK_U8(0x0b12)
#define SMIAPP_REG_U8_DYNAMIC_TRIPLET_DEFECT_CORRECT_ENABLE	SMIAPP_REG_MK_U8(0x0b13)
#define SMIAPP_REG_U8_DYNAMIC_TRIPLET_DEFECT_CORRECT_ADJUST	SMIAPP_REG_MK_U8(0x0b14)
#define SMIAPP_REG_U8_DYNAMIC_LINE_DEFECT_CORRECT_ENABLE	SMIAPP_REG_MK_U8(0x0b15)
#define SMIAPP_REG_U8_DYNAMIC_LINE_DEFECT_CORRECT_ADJUST	SMIAPP_REG_MK_U8(0x0b16)
#define SMIAPP_REG_U8_EDOF_MODE					SMIAPP_REG_MK_U8(0x0b80)
#define SMIAPP_REG_U8_SHARPNESS					SMIAPP_REG_MK_U8(0x0b83)
#define SMIAPP_REG_U8_DENOISING					SMIAPP_REG_MK_U8(0x0b84)
#define SMIAPP_REG_U8_MODULE_SPECIFIC				SMIAPP_REG_MK_U8(0x0b85)
#define SMIAPP_REG_U16_DEPTH_OF_FIELD				SMIAPP_REG_MK_U16(0x0b86)
#define SMIAPP_REG_U16_FOCUS_DISTANCE				SMIAPP_REG_MK_U16(0x0b88)
#define SMIAPP_REG_U8_ESTIMATION_MODE_CTRL			SMIAPP_REG_MK_U8(0x0b8a)
#define SMIAPP_REG_U16_COLOUR_TEMPERATURE			SMIAPP_REG_MK_U16(0x0b8c)
#define SMIAPP_REG_U16_ABSOLUTE_GAIN_GREENR			SMIAPP_REG_MK_U16(0x0b8e)
#define SMIAPP_REG_U16_ABSOLUTE_GAIN_RED			SMIAPP_REG_MK_U16(0x0b90)
#define SMIAPP_REG_U16_ABSOLUTE_GAIN_BLUE			SMIAPP_REG_MK_U16(0x0b92)
#define SMIAPP_REG_U16_ABSOLUTE_GAIN_GREENB			SMIAPP_REG_MK_U16(0x0b94)
#define SMIAPP_REG_U8_ESTIMATION_ZONE_MODE			SMIAPP_REG_MK_U8(0x0bc0)
#define SMIAPP_REG_U16_FIXED_ZONE_WEIGHTING			SMIAPP_REG_MK_U16(0x0bc2)
#define SMIAPP_REG_U16_CUSTOM_ZONE_X_START			SMIAPP_REG_MK_U16(0x0bc4)
#define SMIAPP_REG_U16_CUSTOM_ZONE_Y_START			SMIAPP_REG_MK_U16(0x0bc6)
#define SMIAPP_REG_U16_CUSTOM_ZONE_WIDTH			SMIAPP_REG_MK_U16(0x0bc8)
#define SMIAPP_REG_U16_CUSTOM_ZONE_HEIGHT			SMIAPP_REG_MK_U16(0x0bca)
#define SMIAPP_REG_U8_GLOBAL_RESET_CTRL1			SMIAPP_REG_MK_U8(0x0c00)
#define SMIAPP_REG_U8_GLOBAL_RESET_CTRL2			SMIAPP_REG_MK_U8(0x0c01)
#define SMIAPP_REG_U8_GLOBAL_RESET_MODE_CONFIG_1		SMIAPP_REG_MK_U8(0x0c02)
#define SMIAPP_REG_U8_GLOBAL_RESET_MODE_CONFIG_2		SMIAPP_REG_MK_U8(0x0c03)
#define SMIAPP_REG_U16_TRDY_CTRL				SMIAPP_REG_MK_U16(0x0c04)
#define SMIAPP_REG_U16_TRDOUT_CTRL				SMIAPP_REG_MK_U16(0x0c06)
#define SMIAPP_REG_U16_TSHUTTER_STROBE_DELAY_CTRL		SMIAPP_REG_MK_U16(0x0c08)
#define SMIAPP_REG_U16_TSHUTTER_STROBE_WIDTH_CTRL		SMIAPP_REG_MK_U16(0x0c0a)
#define SMIAPP_REG_U16_TFLASH_STROBE_DELAY_CTRL			SMIAPP_REG_MK_U16(0x0c0c)
#define SMIAPP_REG_U16_TFLASH_STROBE_WIDTH_HIGH_CTRL		SMIAPP_REG_MK_U16(0x0c0e)
#define SMIAPP_REG_U16_TGRST_INTERVAL_CTRL			SMIAPP_REG_MK_U16(0x0c10)
#define SMIAPP_REG_U8_FLASH_STROBE_ADJUSTMENT			SMIAPP_REG_MK_U8(0x0c12)
#define SMIAPP_REG_U16_FLASH_STROBE_START_POINT			SMIAPP_REG_MK_U16(0x0c14)
#define SMIAPP_REG_U16_TFLASH_STROBE_DELAY_RS_CTRL		SMIAPP_REG_MK_U16(0x0c16)
#define SMIAPP_REG_U16_TFLASH_STROBE_WIDTH_HIGH_RS_CTRL		SMIAPP_REG_MK_U16(0x0c18)
#define SMIAPP_REG_U8_FLASH_MODE_RS				SMIAPP_REG_MK_U8(0x0c1a)
#define SMIAPP_REG_U8_FLASH_TRIGGER_RS				SMIAPP_REG_MK_U8(0x0c1b)
#define SMIAPP_REG_U8_FLASH_STATUS				SMIAPP_REG_MK_U8(0x0c1c)
#define SMIAPP_REG_U8_SA_STROBE_MODE				SMIAPP_REG_MK_U8(0x0c1d)
#define SMIAPP_REG_U16_SA_STROBE_START_POINT			SMIAPP_REG_MK_U16(0x0c1e)
#define SMIAPP_REG_U16_TSA_STROBE_DELAY_CTRL			SMIAPP_REG_MK_U16(0x0c20)
#define SMIAPP_REG_U16_TSA_STROBE_WIDTH_CTRL			SMIAPP_REG_MK_U16(0x0c22)
#define SMIAPP_REG_U8_SA_STROBE_TRIGGER				SMIAPP_REG_MK_U8(0x0c24)
#define SMIAPP_REG_U8_SPECIAL_ACTUATOR_STATUS			SMIAPP_REG_MK_U8(0x0c25)
#define SMIAPP_REG_U16_TFLASH_STROBE_WIDTH2_HIGH_RS_CTRL	SMIAPP_REG_MK_U16(0x0c26)
#define SMIAPP_REG_U16_TFLASH_STROBE_WIDTH_LOW_RS_CTRL		SMIAPP_REG_MK_U16(0x0c28)
#define SMIAPP_REG_U8_TFLASH_STROBE_COUNT_RS_CTRL		SMIAPP_REG_MK_U8(0x0c2a)
#define SMIAPP_REG_U8_TFLASH_STROBE_COUNT_CTRL			SMIAPP_REG_MK_U8(0x0c2b)
#define SMIAPP_REG_U16_TFLASH_STROBE_WIDTH2_HIGH_CTRL		SMIAPP_REG_MK_U16(0x0c2c)
#define SMIAPP_REG_U16_TFLASH_STROBE_WIDTH_LOW_CTRL		SMIAPP_REG_MK_U16(0x0c2e)
#define SMIAPP_REG_U8_LOW_LEVEL_CTRL				SMIAPP_REG_MK_U8(0x0c80)
#define SMIAPP_REG_U16_MAIN_TRIGGER_REF_POINT			SMIAPP_REG_MK_U16(0x0c82)
#define SMIAPP_REG_U16_MAIN_TRIGGER_T3				SMIAPP_REG_MK_U16(0x0c84)
#define SMIAPP_REG_U8_MAIN_TRIGGER_COUNT			SMIAPP_REG_MK_U8(0x0c86)
#define SMIAPP_REG_U16_PHASE1_TRIGGER_T3			SMIAPP_REG_MK_U16(0x0c88)
#define SMIAPP_REG_U8_PHASE1_TRIGGER_COUNT			SMIAPP_REG_MK_U8(0x0c8a)
#define SMIAPP_REG_U16_PHASE2_TRIGGER_T3			SMIAPP_REG_MK_U16(0x0c8c)
#define SMIAPP_REG_U8_PHASE2_TRIGGER_COUNT			SMIAPP_REG_MK_U8(0x0c8e)
#define SMIAPP_REG_U8_MECH_SHUTTER_CTRL				SMIAPP_REG_MK_U8(0x0d00)
#define SMIAPP_REG_U8_OPERATION_MODE				SMIAPP_REG_MK_U8(0x0d01)
#define SMIAPP_REG_U8_ACT_STATE1				SMIAPP_REG_MK_U8(0x0d02)
#define SMIAPP_REG_U8_ACT_STATE2				SMIAPP_REG_MK_U8(0x0d03)
#define SMIAPP_REG_U16_FOCUS_CHANGE				SMIAPP_REG_MK_U16(0x0d80)
#define SMIAPP_REG_U16_FOCUS_CHANGE_CONTROL			SMIAPP_REG_MK_U16(0x0d82)
#define SMIAPP_REG_U16_FOCUS_CHANGE_NUMBER_PHASE1		SMIAPP_REG_MK_U16(0x0d84)
#define SMIAPP_REG_U16_FOCUS_CHANGE_NUMBER_PHASE2		SMIAPP_REG_MK_U16(0x0d86)
#define SMIAPP_REG_U8_STROBE_COUNT_PHASE1			SMIAPP_REG_MK_U8(0x0d88)
#define SMIAPP_REG_U8_STROBE_COUNT_PHASE2			SMIAPP_REG_MK_U8(0x0d89)
#define SMIAPP_REG_U8_POSITION					SMIAPP_REG_MK_U8(0x0d8a)
#define SMIAPP_REG_U8_BRACKETING_LUT_CONTROL			SMIAPP_REG_MK_U8(0x0e00)
#define SMIAPP_REG_U8_BRACKETING_LUT_MODE			SMIAPP_REG_MK_U8(0x0e01)
#define SMIAPP_REG_U8_BRACKETING_LUT_ENTRY_CONTROL		SMIAPP_REG_MK_U8(0x0e02)
#define SMIAPP_REG_U8_LUT_PARAMETERS_START			SMIAPP_REG_MK_U8(0x0e10)
#define SMIAPP_REG_U8_LUT_PARAMETERS_END			SMIAPP_REG_MK_U8(0x0eff)
#define SMIAPP_REG_U16_INTEGRATION_TIME_CAPABILITY		SMIAPP_REG_MK_U16(0x1000)
#define SMIAPP_REG_U16_COARSE_INTEGRATION_TIME_MIN		SMIAPP_REG_MK_U16(0x1004)
#define SMIAPP_REG_U16_COARSE_INTEGRATION_TIME_MAX_MARGIN	SMIAPP_REG_MK_U16(0x1006)
#define SMIAPP_REG_U16_FINE_INTEGRATION_TIME_MIN		SMIAPP_REG_MK_U16(0x1008)
#define SMIAPP_REG_U16_FINE_INTEGRATION_TIME_MAX_MARGIN		SMIAPP_REG_MK_U16(0x100a)
#define SMIAPP_REG_U16_DIGITAL_GAIN_CAPABILITY			SMIAPP_REG_MK_U16(0x1080)
#define SMIAPP_REG_U16_DIGITAL_GAIN_MIN				SMIAPP_REG_MK_U16(0x1084)
#define SMIAPP_REG_U16_DIGITAL_GAIN_MAX				SMIAPP_REG_MK_U16(0x1086)
#define SMIAPP_REG_U16_DIGITAL_GAIN_STEP_SIZE			SMIAPP_REG_MK_U16(0x1088)
#define SMIAPP_REG_F32_MIN_EXT_CLK_FREQ_HZ			SMIAPP_REG_MK_F32(0x1100)
#define SMIAPP_REG_F32_MAX_EXT_CLK_FREQ_HZ			SMIAPP_REG_MK_F32(0x1104)
#define SMIAPP_REG_U16_MIN_PRE_PLL_CLK_DIV			SMIAPP_REG_MK_U16(0x1108)
#define SMIAPP_REG_U16_MAX_PRE_PLL_CLK_DIV			SMIAPP_REG_MK_U16(0x110a)
#define SMIAPP_REG_F32_MIN_PLL_IP_FREQ_HZ			SMIAPP_REG_MK_F32(0x110c)
#define SMIAPP_REG_F32_MAX_PLL_IP_FREQ_HZ			SMIAPP_REG_MK_F32(0x1110)
#define SMIAPP_REG_U16_MIN_PLL_MULTIPLIER			SMIAPP_REG_MK_U16(0x1114)
#define SMIAPP_REG_U16_MAX_PLL_MULTIPLIER			SMIAPP_REG_MK_U16(0x1116)
#define SMIAPP_REG_F32_MIN_PLL_OP_FREQ_HZ			SMIAPP_REG_MK_F32(0x1118)
#define SMIAPP_REG_F32_MAX_PLL_OP_FREQ_HZ			SMIAPP_REG_MK_F32(0x111c)
#define SMIAPP_REG_U16_MIN_VT_SYS_CLK_DIV			SMIAPP_REG_MK_U16(0x1120)
#define SMIAPP_REG_U16_MAX_VT_SYS_CLK_DIV			SMIAPP_REG_MK_U16(0x1122)
#define SMIAPP_REG_F32_MIN_VT_SYS_CLK_FREQ_HZ			SMIAPP_REG_MK_F32(0x1124)
#define SMIAPP_REG_F32_MAX_VT_SYS_CLK_FREQ_HZ			SMIAPP_REG_MK_F32(0x1128)
#define SMIAPP_REG_F32_MIN_VT_PIX_CLK_FREQ_HZ			SMIAPP_REG_MK_F32(0x112c)
#define SMIAPP_REG_F32_MAX_VT_PIX_CLK_FREQ_HZ			SMIAPP_REG_MK_F32(0x1130)
#define SMIAPP_REG_U16_MIN_VT_PIX_CLK_DIV			SMIAPP_REG_MK_U16(0x1134)
#define SMIAPP_REG_U16_MAX_VT_PIX_CLK_DIV			SMIAPP_REG_MK_U16(0x1136)
#define SMIAPP_REG_U16_MIN_FRAME_LENGTH_LINES			SMIAPP_REG_MK_U16(0x1140)
#define SMIAPP_REG_U16_MAX_FRAME_LENGTH_LINES			SMIAPP_REG_MK_U16(0x1142)
#define SMIAPP_REG_U16_MIN_LINE_LENGTH_PCK			SMIAPP_REG_MK_U16(0x1144)
#define SMIAPP_REG_U16_MAX_LINE_LENGTH_PCK			SMIAPP_REG_MK_U16(0x1146)
#define SMIAPP_REG_U16_MIN_LINE_BLANKING_PCK			SMIAPP_REG_MK_U16(0x1148)
#define SMIAPP_REG_U16_MIN_FRAME_BLANKING_LINES			SMIAPP_REG_MK_U16(0x114a)
#define SMIAPP_REG_U8_MIN_LINE_LENGTH_PCK_STEP_SIZE		SMIAPP_REG_MK_U8(0x114c)
#define SMIAPP_REG_U16_MIN_OP_SYS_CLK_DIV			SMIAPP_REG_MK_U16(0x1160)
#define SMIAPP_REG_U16_MAX_OP_SYS_CLK_DIV			SMIAPP_REG_MK_U16(0x1162)
#define SMIAPP_REG_F32_MIN_OP_SYS_CLK_FREQ_HZ			SMIAPP_REG_MK_F32(0x1164)
#define SMIAPP_REG_F32_MAX_OP_SYS_CLK_FREQ_HZ			SMIAPP_REG_MK_F32(0x1168)
#define SMIAPP_REG_U16_MIN_OP_PIX_CLK_DIV			SMIAPP_REG_MK_U16(0x116c)
#define SMIAPP_REG_U16_MAX_OP_PIX_CLK_DIV			SMIAPP_REG_MK_U16(0x116e)
#define SMIAPP_REG_F32_MIN_OP_PIX_CLK_FREQ_HZ			SMIAPP_REG_MK_F32(0x1170)
#define SMIAPP_REG_F32_MAX_OP_PIX_CLK_FREQ_HZ			SMIAPP_REG_MK_F32(0x1174)
#define SMIAPP_REG_U16_X_ADDR_MIN				SMIAPP_REG_MK_U16(0x1180)
#define SMIAPP_REG_U16_Y_ADDR_MIN				SMIAPP_REG_MK_U16(0x1182)
#define SMIAPP_REG_U16_X_ADDR_MAX				SMIAPP_REG_MK_U16(0x1184)
#define SMIAPP_REG_U16_Y_ADDR_MAX				SMIAPP_REG_MK_U16(0x1186)
#define SMIAPP_REG_U16_MIN_X_OUTPUT_SIZE			SMIAPP_REG_MK_U16(0x1188)
#define SMIAPP_REG_U16_MIN_Y_OUTPUT_SIZE			SMIAPP_REG_MK_U16(0x118a)
#define SMIAPP_REG_U16_MAX_X_OUTPUT_SIZE			SMIAPP_REG_MK_U16(0x118c)
#define SMIAPP_REG_U16_MAX_Y_OUTPUT_SIZE			SMIAPP_REG_MK_U16(0x118e)
#define SMIAPP_REG_U16_MIN_EVEN_INC				SMIAPP_REG_MK_U16(0x11c0)
#define SMIAPP_REG_U16_MAX_EVEN_INC				SMIAPP_REG_MK_U16(0x11c2)
#define SMIAPP_REG_U16_MIN_ODD_INC				SMIAPP_REG_MK_U16(0x11c4)
#define SMIAPP_REG_U16_MAX_ODD_INC				SMIAPP_REG_MK_U16(0x11c6)
#define SMIAPP_REG_U16_SCALING_CAPABILITY			SMIAPP_REG_MK_U16(0x1200)
#define SMIAPP_REG_U16_SCALER_M_MIN				SMIAPP_REG_MK_U16(0x1204)
#define SMIAPP_REG_U16_SCALER_M_MAX				SMIAPP_REG_MK_U16(0x1206)
#define SMIAPP_REG_U16_SCALER_N_MIN				SMIAPP_REG_MK_U16(0x1208)
#define SMIAPP_REG_U16_SCALER_N_MAX				SMIAPP_REG_MK_U16(0x120a)
#define SMIAPP_REG_U16_SPATIAL_SAMPLING_CAPABILITY		SMIAPP_REG_MK_U16(0x120c)
#define SMIAPP_REG_U8_DIGITAL_CROP_CAPABILITY			SMIAPP_REG_MK_U8(0x120e)
#define SMIAPP_REG_U16_COMPRESSION_CAPABILITY			SMIAPP_REG_MK_U16(0x1300)
#define SMIAPP_REG_U16_MATRIX_ELEMENT_REDINRED			SMIAPP_REG_MK_U16(0x1400)
#define SMIAPP_REG_U16_MATRIX_ELEMENT_GREENINRED		SMIAPP_REG_MK_U16(0x1402)
#define SMIAPP_REG_U16_MATRIX_ELEMENT_BLUEINRED			SMIAPP_REG_MK_U16(0x1404)
#define SMIAPP_REG_U16_MATRIX_ELEMENT_REDINGREEN		SMIAPP_REG_MK_U16(0x1406)
#define SMIAPP_REG_U16_MATRIX_ELEMENT_GREENINGREEN		SMIAPP_REG_MK_U16(0x1408)
#define SMIAPP_REG_U16_MATRIX_ELEMENT_BLUEINGREEN		SMIAPP_REG_MK_U16(0x140a)
#define SMIAPP_REG_U16_MATRIX_ELEMENT_REDINBLUE			SMIAPP_REG_MK_U16(0x140c)
#define SMIAPP_REG_U16_MATRIX_ELEMENT_GREENINBLUE		SMIAPP_REG_MK_U16(0x140e)
#define SMIAPP_REG_U16_MATRIX_ELEMENT_BLUEINBLUE		SMIAPP_REG_MK_U16(0x1410)
#define SMIAPP_REG_U16_FIFO_SIZE_PIXELS				SMIAPP_REG_MK_U16(0x1500)
#define SMIAPP_REG_U8_FIFO_SUPPORT_CAPABILITY			SMIAPP_REG_MK_U8(0x1502)
#define SMIAPP_REG_U8_DPHY_CTRL_CAPABILITY			SMIAPP_REG_MK_U8(0x1600)
#define SMIAPP_REG_U8_CSI_LANE_MODE_CAPABILITY			SMIAPP_REG_MK_U8(0x1601)
#define SMIAPP_REG_U8_CSI_SIGNALLING_MODE_CAPABILITY		SMIAPP_REG_MK_U8(0x1602)
#define SMIAPP_REG_U8_FAST_STANDBY_CAPABILITY			SMIAPP_REG_MK_U8(0x1603)
#define SMIAPP_REG_U8_CCI_ADDRESS_CONTROL_CAPABILITY		SMIAPP_REG_MK_U8(0x1604)
#define SMIAPP_REG_U32_MAX_PER_LANE_BITRATE_1_LANE_MODE_MBPS	SMIAPP_REG_MK_U32(0x1608)
#define SMIAPP_REG_U32_MAX_PER_LANE_BITRATE_2_LANE_MODE_MBPS	SMIAPP_REG_MK_U32(0x160c)
#define SMIAPP_REG_U32_MAX_PER_LANE_BITRATE_3_LANE_MODE_MBPS	SMIAPP_REG_MK_U32(0x1610)
#define SMIAPP_REG_U32_MAX_PER_LANE_BITRATE_4_LANE_MODE_MBPS	SMIAPP_REG_MK_U32(0x1614)
#define SMIAPP_REG_U8_TEMP_SENSOR_CAPABILITY			SMIAPP_REG_MK_U8(0x1618)
#define SMIAPP_REG_U16_MIN_FRAME_LENGTH_LINES_BIN		SMIAPP_REG_MK_U16(0x1700)
#define SMIAPP_REG_U16_MAX_FRAME_LENGTH_LINES_BIN		SMIAPP_REG_MK_U16(0x1702)
#define SMIAPP_REG_U16_MIN_LINE_LENGTH_PCK_BIN			SMIAPP_REG_MK_U16(0x1704)
#define SMIAPP_REG_U16_MAX_LINE_LENGTH_PCK_BIN			SMIAPP_REG_MK_U16(0x1706)
#define SMIAPP_REG_U16_MIN_LINE_BLANKING_PCK_BIN		SMIAPP_REG_MK_U16(0x1708)
#define SMIAPP_REG_U16_FINE_INTEGRATION_TIME_MIN_BIN		SMIAPP_REG_MK_U16(0x170a)
#define SMIAPP_REG_U16_FINE_INTEGRATION_TIME_MAX_MARGIN_BIN	SMIAPP_REG_MK_U16(0x170c)
#define SMIAPP_REG_U8_BINNING_CAPABILITY			SMIAPP_REG_MK_U8(0x1710)
#define SMIAPP_REG_U8_BINNING_WEIGHTING_CAPABILITY		SMIAPP_REG_MK_U8(0x1711)
#define SMIAPP_REG_U8_BINNING_SUBTYPES				SMIAPP_REG_MK_U8(0x1712)
#define SMIAPP_REG_U8_BINNING_TYPE_n(n)				SMIAPP_REG_MK_U8(0x1713 + (n)) /* 1 <= n <= 237 */
#define SMIAPP_REG_U8_DATA_TRANSFER_IF_CAPABILITY		SMIAPP_REG_MK_U8(0x1800)
#define SMIAPP_REG_U8_SHADING_CORRECTION_CAPABILITY		SMIAPP_REG_MK_U8(0x1900)
#define SMIAPP_REG_U8_GREEN_IMBALANCE_CAPABILITY		SMIAPP_REG_MK_U8(0x1901)
#define SMIAPP_REG_U8_BLACK_LEVEL_CAPABILITY			SMIAPP_REG_MK_U8(0x1902)
#define SMIAPP_REG_U8_MODULE_SPECIFIC_CORRECTION_CAPABILITY	SMIAPP_REG_MK_U8(0x1903)
#define SMIAPP_REG_U16_DEFECT_CORRECTION_CAPABILITY		SMIAPP_REG_MK_U16(0x1904)
#define SMIAPP_REG_U16_DEFECT_CORRECTION_CAPABILITY_2		SMIAPP_REG_MK_U16(0x1906)
#define SMIAPP_REG_U8_EDOF_CAPABILITY				SMIAPP_REG_MK_U8(0x1980)
#define SMIAPP_REG_U8_ESTIMATION_FRAMES				SMIAPP_REG_MK_U8(0x1981)
#define SMIAPP_REG_U8_SUPPORTS_SHARPNESS_ADJ			SMIAPP_REG_MK_U8(0x1982)
#define SMIAPP_REG_U8_SUPPORTS_DENOISING_ADJ			SMIAPP_REG_MK_U8(0x1983)
#define SMIAPP_REG_U8_SUPPORTS_MODULE_SPECIFIC_ADJ		SMIAPP_REG_MK_U8(0x1984)
#define SMIAPP_REG_U8_SUPPORTS_DEPTH_OF_FIELD_ADJ		SMIAPP_REG_MK_U8(0x1985)
#define SMIAPP_REG_U8_SUPPORTS_FOCUS_DISTANCE_ADJ		SMIAPP_REG_MK_U8(0x1986)
#define SMIAPP_REG_U8_COLOUR_FEEDBACK_CAPABILITY		SMIAPP_REG_MK_U8(0x1987)
#define SMIAPP_REG_U8_EDOF_SUPPORT_AB_NXM			SMIAPP_REG_MK_U8(0x1988)
#define SMIAPP_REG_U8_ESTIMATION_MODE_CAPABILITY		SMIAPP_REG_MK_U8(0x19c0)
#define SMIAPP_REG_U8_ESTIMATION_ZONE_CAPABILITY		SMIAPP_REG_MK_U8(0x19c1)
#define SMIAPP_REG_U16_EST_DEPTH_OF_FIELD			SMIAPP_REG_MK_U16(0x19c2)
#define SMIAPP_REG_U16_EST_FOCUS_DISTANCE			SMIAPP_REG_MK_U16(0x19c4)
#define SMIAPP_REG_U16_CAPABILITY_TRDY_MIN			SMIAPP_REG_MK_U16(0x1a00)
#define SMIAPP_REG_U8_FLASH_MODE_CAPABILITY			SMIAPP_REG_MK_U8(0x1a02)
#define SMIAPP_REG_U16_MECH_SHUT_AND_ACT_START_ADDR		SMIAPP_REG_MK_U16(0x1b02)
#define SMIAPP_REG_U8_ACTUATOR_CAPABILITY			SMIAPP_REG_MK_U8(0x1b04)
#define SMIAPP_REG_U16_ACTUATOR_TYPE				SMIAPP_REG_MK_U16(0x1b40)
#define SMIAPP_REG_U8_AF_DEVICE_ADDRESS				SMIAPP_REG_MK_U8(0x1b42)
#define SMIAPP_REG_U16_FOCUS_CHANGE_ADDRESS			SMIAPP_REG_MK_U16(0x1b44)
#define SMIAPP_REG_U8_BRACKETING_LUT_CAPABILITY_1		SMIAPP_REG_MK_U8(0x1c00)
#define SMIAPP_REG_U8_BRACKETING_LUT_CAPABILITY_2		SMIAPP_REG_MK_U8(0x1c01)
#define SMIAPP_REG_U8_BRACKETING_LUT_SIZE			SMIAPP_REG_MK_U8(0x1c02)
