/*
    Hantek 2D72 handheld oscillosope tool for linux
    Copyright (C) 2021 Luca Oliva

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef _HANTEK_H
#define _HANTEK_H

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <libusb.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <assert.h>

#define VENDOR 0x0483
#define PRODUCT 0x2d42

#define FUNC_SCOPE_SETTING              0x0000
#define FUNC_SCOPE_CAPTURE              0x0100
#define FUNC_AWG_SETTING                0x0002
#define FUNC_SCREEN_SETTING             0x0003

//Scope Settings

#define SCOPE_ENABLE_CH1                0x00
#define SCOPE_COUPLING_CH1              0x01
#define SCOPE_PROBEX_CH1                0x02
#define SCOPE_BWLIMIT_CH1               0x03
#define SCOPE_SCALE_CH1                 0x04
#define SCOPE_OFFSET_CH1                0x05

#define SCOPE_ENABLE_CH2                0x06
#define SCOPE_COUPLING_CH2              0x07
#define SCOPE_PROBEX_CH2                0x08
#define SCOPE_BWLIMIT_CH2               0x09
#define SCOPE_SCALE_CH2                 0x0A
#define SCOPE_OFFSET_CH2                0x0B

#define SCOPE_START                     0x0C

#define SCOPE_SCALE_TIME                0x0E
#define SCOPE_OFFSET_TIME               0x0F

#define SCOPE_TRIGGER_SOURCE            0x10
#define SCOPE_TRIGGER_SLOPE             0x11
#define SCOPE_TRIGGER_MODE              0x12
#define SCOPE_TRIGGER_LEVEL             0x14

#define SCOPE_AUTO_SETTING              0x13

#define SCOPE_START_RECV                0x16

#define SCOPE_VAL_COUPLING_AC           0x00
#define SCOPE_VAL_COUPLING_DC           0x01
#define SCOPE_VAL_COUPLING_GND          0x02

#define SCOPE_VAL_PROBEX1               0x00
#define SCOPE_VAL_PROBEX10              0x01
#define SCOPE_VAL_PROBEX100             0x02
#define SCOPE_VAL_PROBEX1000            0x03

#define SCOPE_VAL_SCALE_10mV            0x00
#define SCOPE_VAL_SCALE_20mV            0x01
#define SCOPE_VAL_SCALE_50mV            0x02
#define SCOPE_VAL_SCALE_100mV           0x03
#define SCOPE_VAL_SCALE_200mV           0x04
#define SCOPE_VAL_SCALE_500mV           0x05
#define SCOPE_VAL_SCALE_1V              0x06
#define SCOPE_VAL_SCALE_2V              0x07
#define SCOPE_VAL_SCALE_5V              0x08
#define SCOPE_VAL_SCALE_10V             0x09

#define SCOPE_VAL_SCALE_TIME_5ns        0x00
#define SCOPE_VAL_SCALE_TIME_10ns       0x01
#define SCOPE_VAL_SCALE_TIME_20ns       0x02
#define SCOPE_VAL_SCALE_TIME_50ns       0x03
#define SCOPE_VAL_SCALE_TIME_100ns      0x04
#define SCOPE_VAL_SCALE_TIME_200ns      0x05
#define SCOPE_VAL_SCALE_TIME_500ns      0x06
#define SCOPE_VAL_SCALE_TIME_1us        0x07
#define SCOPE_VAL_SCALE_TIME_2us        0x08
#define SCOPE_VAL_SCALE_TIME_5us        0x09
#define SCOPE_VAL_SCALE_TIME_10us       0x0a
#define SCOPE_VAL_SCALE_TIME_20us       0x0b
#define SCOPE_VAL_SCALE_TIME_50us       0x0c
#define SCOPE_VAL_SCALE_TIME_100us      0x0d
#define SCOPE_VAL_SCALE_TIME_200us      0x0e
#define SCOPE_VAL_SCALE_TIME_500us      0x0f
#define SCOPE_VAL_SCALE_TIME_1ms        0x10
#define SCOPE_VAL_SCALE_TIME_2ms        0x11
#define SCOPE_VAL_SCALE_TIME_5ms        0x12
#define SCOPE_VAL_SCALE_TIME_10ms       0x13
#define SCOPE_VAL_SCALE_TIME_20ms       0x14
#define SCOPE_VAL_SCALE_TIME_50ms       0x15
#define SCOPE_VAL_SCALE_TIME_100ms      0x16
#define SCOPE_VAL_SCALE_TIME_200ms      0x17
#define SCOPE_VAL_SCALE_TIME_500ms      0x18
#define SCOPE_VAL_SCALE_TIME_1s         0x19
#define SCOPE_VAL_SCALE_TIME_2s         0x1a
#define SCOPE_VAL_SCALE_TIME_5s         0x1b
#define SCOPE_VAL_SCALE_TIME_10s        0x1c
#define SCOPE_VAL_SCALE_TIME_20s        0x1d
#define SCOPE_VAL_SCALE_TIME_50s        0x1e
#define SCOPE_VAL_SCALE_TIME_100s       0x1f
#define SCOPE_VAL_SCALE_TIME_200s       0x20
#define SCOPE_VAL_SCALE_TIME_500s       0x21

#define SCOPE_VAL_TRIGGER_SLOPE_RISING  0x00
#define SCOPE_VAL_TRIGGER_SLOPE_FALLING 0x01
#define SCOPE_VAL_TRIGGER_SLOPE_BOTH    0x02

#define SCOPE_VAL_TRIGGER_MODE_AUTO     0x00
#define SCOPE_VAL_TRIGGER_MODE_NORMAL   0x01
#define SCOPE_VAL_TRIGGER_MODE_SINGLE   0x02

//Awg Settings
#define AWG_TYPE                        0x00
#define AWG_FREQ                        0x01
#define AWG_AMP                         0x02
#define AWG_OFF                         0x03
#define AWG_SQUARE_DUTY                 0x04
#define AWG_RAMP_DUTY                   0x05
#define AWG_TRAP_DUTY                   0x06
#define AWG_START                       0x08

#define AWG_VAL_TYPE_SQUARE             0x00
#define AWG_VAL_TYPE_RAMP               0x01
#define AWG_VAL_TYPE_SIN                0x02
#define AWG_VAL_TYPE_TRAP               0x03
#define AWG_VAL_TYPE_ARB1               0x04
#define AWG_VAL_TYPE_ARB2               0x05
#define AWG_VAL_TYPE_ARB3               0x06
#define AWG_VAL_TYPE_ARB4               0x07

//Screen Settings
#define SCREEN_VAL_SCOPE                0x00
#define SCREEN_VAL_DMM                  0x01
#define SCREEN_VAL_AWG                  0x02

GtkRadioButton* scope_radio     = NULL;
GtkRadioButton* awg_radio       = NULL;
GtkRadioButton* dmm_radio       = NULL;

GtkAdjustment* channel_offset_adj_ch1 = NULL;
GtkAdjustment* channel_offset_adj_ch2 = NULL;
GtkAdjustment* trigger_level_adj = NULL;
GtkAdjustment* time_offset_adj = NULL;

GtkListStore* liststore_1x = NULL;
GtkListStore* liststore_10x = NULL;
GtkListStore* liststore_100x = NULL;
GtkListStore* liststore_1000x = NULL;

GtkSwitch* channel_enable_switch_ch1 = NULL;
GtkSwitch* channel_enable_switch_ch2 = NULL;

GtkComboBox* channel_coupling_combobox_ch1 = NULL;
GtkComboBox* channel_coupling_combobox_ch2 = NULL;

GtkComboBox* channel_scale_combobox_ch1 = NULL;
GtkComboBox* channel_scale_combobox_ch2 = NULL;

GtkComboBox* channel_probe_combobox_ch1 = NULL;
GtkComboBox* channel_probe_combobox_ch2 = NULL;

GtkSpinButton* channel_offset_spinbutton_ch1 = NULL;
GtkSpinButton* channel_offset_spinbutton_ch2 = NULL;

GtkSwitch* channel_bwlimit_switch_ch1 = NULL;
GtkSwitch* channel_bwlimit_switch_ch2 = NULL;

GtkComboBox*    time_scale_combobox     = NULL;
GtkSpinButton*  time_offset_spinbutton  = NULL;

GtkComboBox*    trigger_source_combobox    = NULL;
GtkComboBox*    trigger_slope_combobox     = NULL;
GtkComboBox*    trigger_mode_combobox      = NULL;
GtkSpinButton*  trigger_level_spinbutton = NULL;

GtkComboBox*    awg_type_combobox           = NULL;
GtkSpinButton*  awg_frequency_spinbutton    = NULL;
GtkSpinButton*  awg_amplitude_spinbutton    = NULL;
GtkSpinButton*  awg_offset_spinbutton       = NULL;
GtkSpinButton*  awg_squareduty_spinbutton   = NULL;
GtkSpinButton*  awg_rampduty_spinbutton     = NULL;
GtkSpinButton*  awg_trapriseduty_spinbutton = NULL;
GtkSpinButton*  awg_traphighduty_spinbutton = NULL;
GtkSpinButton*  awg_trapfallduty_spinbutton = NULL;

GtkSpinButton*  capture_samples_spinbutton  = NULL;

GtkWidget* drawing_area = NULL;

libusb_device_handle *handle = NULL;

typedef struct  __attribute__((packed)) {
        uint8_t         idx;
        uint8_t         boh;
        uint16_t        func;
        uint8_t         cmd;
        union {
                uint8_t  val[4];
                uint16_t size[2];
                uint32_t val32;
        };
        uint8_t last;
} Hantek_command_t ;

uint8_t capture_buffer[6000];

typedef struct {
        bool    channel_enable[2];
        int     channel_coupling[2];
        int     channel_probe[2];
        int     channel_scale[2];
        float   channel_offset[2];
        bool    channel_bwlimit[2];

        int     time_scale;
        float   time_offset;

        int     trigger_source;
        int     trigger_slope;
        int     trigger_mode;
        float   trigger_level;                        

        int     awg_type;
        float   awg_frequency;
        float   awg_amplitude;
        float   awg_offset;
        float   awg_squareduty;
        float   awg_rampduty;
        float   awg_trapriseduty;
        float   awg_traphighduty;
        float   awg_trapfallduty;

        int     num_samples;
} config_t;

config_t default_config = {
        .channel_enable   = { true, true },
        .channel_coupling = { 0, 0 },
        .channel_probe    = { 0, 0 },
        .channel_scale    = { 0, 0 },
        .channel_offset   = { 0, 0 },
        .channel_bwlimit  = { true, true },

        .time_scale       = 0,
        .time_offset      = 0,

        .trigger_source   = 0,
        .trigger_slope    = 0,
        .trigger_mode     = 0,
        .trigger_level    = 0,

        .awg_type         = 0,
        .awg_frequency    = 1000,
        .awg_amplitude    = 2.5,
        .awg_offset       = 0,
        .awg_squareduty   = 0.5,
        .awg_rampduty     = 0.5,
        .awg_trapriseduty = 0.1,
        .awg_traphighduty = 0.4,
        .awg_trapfallduty = 0.1,

        .num_samples      = 1200 
};

config_t* cur_config = NULL;

#endif //_HANTEK_H
