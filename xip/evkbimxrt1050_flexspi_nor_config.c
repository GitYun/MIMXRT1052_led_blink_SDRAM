/*
 * Copyright 2017-2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "evkbimxrt1050_flexspi_nor_config.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.xip_board"
#endif

/*******************************************************************************
 * Code
 ******************************************************************************/
#if defined(XIP_BOOT_HEADER_ENABLE) && (XIP_BOOT_HEADER_ENABLE == 1)
#if defined(__CC_ARM) || defined(__ARMCC_VERSION) || defined(__GNUC__)
__attribute__((section(".boot_hdr.conf"), used))
#elif defined(__ICCARM__)
#pragma location = ".boot_hdr.conf"
#endif

const flexspi_nor_config_t spiflash_config = {
    .memConfig =
        {
            .tag                = FLEXSPI_CFG_BLK_TAG,      //  ASCII: 'FCFB'

            // [31:24]: ASCII 'V', [23:16]: major = 1, [15:8]: minor = 4, [7:0]: bugfix = 0
            .version            = FLEXSPI_CFG_BLK_VERSION,

            .readSampleClkSrc   = kFlexSPIReadSampleClk_LoopbackInternally,
            .csHoldTime         = 3u, // Serial Flash CS Hold Time Recommend default value is 0x03
            .csSetupTime        = 3u, // Serial Flash CS Setup Time Recommend default value is 0x03
            .columnAddressWidth = 0u, // 3 - For HyperFlash, 12/13 - For Serial NAND, 0 - Other devices
            .deviceModeCfgEnable= 1u,

            .deviceModeType = 1u, // Quad Enable

            // Wait time for all configuration commands, unit 100us
            // If it is greater than 0, ROM will wait (waitTimeCfgCommands * 100us)
            // for all device memory configuration commands instead of using read
            // status to wait until these commands complete.
            .waitTimeCfgCommands = 0, // Available for device that support v1.1.0 FlexSPI configuration block

            // Set QE bit at Status Register of Nor Flash
            // Now configure NOR flash SPI mode use specified LUT sequence
            .deviceModeSeq.seqNum   = 1, // Bit[7:0] - number of LUT sequences for Device mode configuration command
            .deviceModeSeq.seqId    = 2, // Bit[15:8] - starting LUT index of Device mode configuration command
            .deviceModeSeq.reserved = 0, // Bit[31:16] - must be 0
            // [31:24] = xx (unused); [23:0] = S23:S0 (Status Register 3/2/1, S9 is Quad Enable bit)
            .deviceModeArg          = 0x00000200, // Device Mode argument, effective only when `deviceModeCfgEnable = 1`

            .controllerMiscOption   = 0, // For DDR, now unused

            .deviceType    = kFlexSpiDeviceType_SerialNOR, // 1 - Serial NOR Flash; 2 - Serial NAND Flash
            .sflashPadType = kSerialFlash_4Pads,           // 4 Pin for SPI Data (Quad SPI)
            .serialClkFreq = kFlexSpiSerialClk_133MHz,

            // 0 - Use pre-defined LUT sequence index and number
            // 1 - Use LUT sequence parameters provided in this block
            .lutCustomSeqEnable   = 0,

            // For SPI NOR, need to fill with actual size;
            // For SPI NAND, need to fill with actual size * 2
            .sflashA1Size  = 32u * 1024u * 1024u,

            .busyOffset      = 0, // busy bit offset, valid range :0-31
            .busyBitPolarity = 0, // 0 - busy bit is 1 if device is busy

            .lookupTable =
            {
                 // 0th Seq: Fast Read Quad I/O  with 4-Byte Address
                 [4 * CMD_LUT_SEQ_IDX_READ + 0] = FLEXSPI_LUT_SEQ(CMD_SDR,   FLEXSPI_1PAD, 0xEC, RADDR_SDR, FLEXSPI_4PAD, 0x20),
                 [4 * CMD_LUT_SEQ_IDX_READ + 1] = FLEXSPI_LUT_SEQ(MODE8_SDR, FLEXSPI_4PAD, 0xFF, DUMMY_SDR, FLEXSPI_4PAD, 0x04),
                 [4 * CMD_LUT_SEQ_IDX_READ + 2] = FLEXSPI_LUT_SEQ(READ_SDR,  FLEXSPI_4PAD, 0x01, STOP,      FLEXSPI_1PAD, 0x00),
                 [4 * CMD_LUT_SEQ_IDX_READ + 3] = 0,

                 // 1th Seq: Read Status Register 1
                 [4 * CMD_LUT_SEQ_IDX_READSTATUS + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x05, READ_SDR, FLEXSPI_1PAD, 0x01),

                 // 2th Seq: Write Status Register 2
                 [4 * 2 + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x31, WRITE_SDR, FLEXSPI_1PAD, 0x01),

                 // 3th Seq: Write Enable
                 [4 * CMD_LUT_SEQ_IDX_WRITEENABLE + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x06, STOP, FLEXSPI_1PAD, 0x00),

                 // 5th Seq: Sector Erase with 4-Bytes Address
                 [4 * NOR_CMD_LUT_SEQ_IDX_ERASESECTOR + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x21, RADDR_SDR, FLEXSPI_1PAD, 0x20),

                 // 9th Seq: Qaud Page Program with 4-Bytes Address
                 [4 * CMD_LUT_SEQ_IDX_WRITE + 0] = FLEXSPI_LUT_SEQ(CMD_SDR,     FLEXSPI_1PAD, 0x34, RADDR_SDR, FLEXSPI_4PAD, 0x20),
                 [4 * CMD_LUT_SEQ_IDX_WRITE + 1] = FLEXSPI_LUT_SEQ(WRITE_SDR,   FLEXSPI_4PAD, 0x01, STOP,      FLEXSPI_1PAD, 0x00),

                 // 11th Seq: Chip Erase
                 [4 * NOR_CMD_LUT_SEQ_IDX_CHIPERASE + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0xC7, STOP, FLEXSPI_1PAD, 0x00),
            },

            .lutCustomSeq = {{0}},
        },
    .pageSize           = 256u,
    .sectorSize         = 4u * 1024u,
    .blockSize          = 64u * 1024u,
    .ipcmdSerialClkFreq = 0, // 0 - No change, keep current serial clock unchanged
//    .isCmdSerialClkFreq = kFlexSpiSerialClk_133MHz,
};
#endif /* XIP_BOOT_HEADER_ENABLE */
