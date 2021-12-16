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

#include "Hantek.h"

int find_device(int vendor, int product, libusb_device **output, libusb_device_handle **handle) {
    libusb_device **list;
    libusb_device *device = NULL;
    struct libusb_device_descriptor desc;

    int status = 0, res = 0;
    ssize_t i = 0;
    ssize_t cnt = libusb_get_device_list(NULL, &list);

    *output = NULL;
    *handle = NULL;

    if(cnt < 0) {
        fprintf(stderr, "No USB devices discovered.\n");
        status = 1;
        goto cleanup;
    }

    for(i = 0; i < cnt; ++i) {
        device = list[i];
        res = libusb_get_device_descriptor(device, &desc);

        if(res != 0) {
            fprintf(stderr, "[%d] Error getting device descriptor.\n", res);
            continue;
        }

        if(desc.idVendor == vendor && desc.idProduct == product) {
            *output = device;
            break;
        }
    }

    if(*output != NULL) {
        res = libusb_open(device, handle);
        if(res == LIBUSB_SUCCESS) {
            status = 0;
        }else{
            status = 2;
            fprintf(stderr, "[%d] Failed opening USB device.\n", res);
        }
    }else{
        status = 3;
    }

cleanup:
    libusb_free_device_list(list, 1);
    return status;
}

int claim_interfaces(libusb_device *device, libusb_device_handle *handle) {
    int i = 0, res = 0;
    struct libusb_config_descriptor *config = NULL;

    res = libusb_get_config_descriptor(device, 0, &config);
    if(res != 0) {
        goto cleanup;
    }

    for(i = 0; i < config->bNumInterfaces; ++i) {
        res = libusb_claim_interface(handle, i);
        if(res != LIBUSB_SUCCESS) {
            for(i; i >= 0; --i) {
                libusb_release_interface(handle, i);
            }

            fprintf(stderr, "[%d] Failed claiming interface.", res);
            break;
        }
    }

cleanup:
    libusb_free_config_descriptor(config);
    return res;
}

int release_interfaces(libusb_device *device, libusb_device_handle *handle) {
    int i = 0, res = 0;
    struct libusb_config_descriptor *config = NULL;

    res = libusb_get_config_descriptor(device, 0, &config);
    if(res != 0) {
        goto cleanup;
    }

    for(i = 0; i < config->bNumInterfaces; ++i) {
        libusb_release_interface(handle, i);
    }

cleanup:
    libusb_free_config_descriptor(config);
    return res;
}

int main(int argc, char *argv[]) {
    libusb_device *device = NULL;
    int status = 0;
    int cfg_fd;

    GtkBuilder      *builder;
    GtkWidget       *window;

    libusb_init(NULL);

    status = find_device(VENDOR, PRODUCT, &device, &handle);
    if(status != 0) {
        fprintf(stderr, "[%d] Failed to find device.\n", status);
        goto cleanup;
    }

    claim_interfaces(device, handle);

    gtk_init(&argc, &argv);

    builder = gtk_builder_new_from_file("Hantek.glade");

    window = GTK_WIDGET(gtk_builder_get_object(builder,"window_main"));

    scope_radio                     = GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "scope_radio"));
    awg_radio                       = GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "awg_radio"));
    dmm_radio                       = GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "dmm_radio"));

    channel_offset_adj_ch1          = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "channel_offset_adj_ch1"));
    channel_offset_adj_ch2          = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "channel_offset_adj_ch2"));
    trigger_level_adj               = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "trigger_level_adj"));
    time_offset_adj                 = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "time_offset_adj"));

    liststore_1x                    = GTK_LIST_STORE(gtk_builder_get_object(builder, "liststore_1x"));
    liststore_10x                   = GTK_LIST_STORE(gtk_builder_get_object(builder, "liststore_10x"));
    liststore_100x                  = GTK_LIST_STORE(gtk_builder_get_object(builder, "liststore_100x"));
    liststore_1000x                 = GTK_LIST_STORE(gtk_builder_get_object(builder, "liststore_1000x"));

    channel_enable_switch_ch1       = GTK_SWITCH(gtk_builder_get_object(builder, "channel_enable_switch_ch1"));
    channel_enable_switch_ch2       = GTK_SWITCH(gtk_builder_get_object(builder, "channel_enable_switch_ch2"));

    channel_coupling_combobox_ch1   = GTK_COMBO_BOX(gtk_builder_get_object(builder, "channel_coupling_combobox_ch1"));
    channel_coupling_combobox_ch2   = GTK_COMBO_BOX(gtk_builder_get_object(builder, "channel_coupling_combobox_ch2"));

    channel_scale_combobox_ch1      = GTK_COMBO_BOX(gtk_builder_get_object(builder, "channel_scale_combobox_ch1"));
    channel_scale_combobox_ch2      = GTK_COMBO_BOX(gtk_builder_get_object(builder, "channel_scale_combobox_ch2"));

    channel_probe_combobox_ch1      = GTK_COMBO_BOX(gtk_builder_get_object(builder, "channel_probe_combobox_ch1"));
    channel_probe_combobox_ch2      = GTK_COMBO_BOX(gtk_builder_get_object(builder, "channel_probe_combobox_ch2"));

    channel_offset_spinbutton_ch1   = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "channel_offset_spinbutton_ch1"));
    channel_offset_spinbutton_ch2   = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "channel_offset_spinbutton_ch2"));

    channel_bwlimit_switch_ch1      = GTK_SWITCH(gtk_builder_get_object(builder, "channel_bwlimit_switch_ch1"));
    channel_bwlimit_switch_ch2      = GTK_SWITCH(gtk_builder_get_object(builder, "channel_bwlimit_switch_ch2"));

    time_scale_combobox             = GTK_COMBO_BOX(gtk_builder_get_object(builder,   "time_scale_combobox"));
    time_offset_spinbutton          = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "time_offset_spinbutton"));

    trigger_source_combobox         = GTK_COMBO_BOX(gtk_builder_get_object(builder,   "trigger_source_combobox"));
    trigger_slope_combobox          = GTK_COMBO_BOX(gtk_builder_get_object(builder,   "trigger_slope_combobox"));
    trigger_mode_combobox           = GTK_COMBO_BOX(gtk_builder_get_object(builder,   "trigger_mode_combobox"));
    trigger_level_spinbutton        = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "trigger_level_spinbutton"));

    awg_type_combobox               = GTK_COMBO_BOX(gtk_builder_get_object(builder,     "awg_type_combobox"));
    awg_frequency_spinbutton        = GTK_SPIN_BUTTON(gtk_builder_get_object(builder,   "awg_frequency_spinbutton"));
    awg_amplitude_spinbutton        = GTK_SPIN_BUTTON(gtk_builder_get_object(builder,   "awg_amplitude_spinbutton"));
    awg_offset_spinbutton           = GTK_SPIN_BUTTON(gtk_builder_get_object(builder,   "awg_offset_spinbutton"));
    awg_squareduty_spinbutton       = GTK_SPIN_BUTTON(gtk_builder_get_object(builder,   "awg_squareduty_spinbutton"));
    awg_rampduty_spinbutton         = GTK_SPIN_BUTTON(gtk_builder_get_object(builder,   "awg_rampduty_spinbutton"));
    awg_trapriseduty_spinbutton     = GTK_SPIN_BUTTON(gtk_builder_get_object(builder,   "awg_trapriseduty_spinbutton"));
    awg_traphighduty_spinbutton     = GTK_SPIN_BUTTON(gtk_builder_get_object(builder,   "awg_traphighduty_spinbutton"));
    awg_trapfallduty_spinbutton     = GTK_SPIN_BUTTON(gtk_builder_get_object(builder,   "awg_trapfallduty_spinbutton"));

    capture_samples_spinbutton      = GTK_SPIN_BUTTON(gtk_builder_get_object(builder,   "capture_samples_spinbutton"));

    drawing_area                    = GTK_WIDGET(gtk_builder_get_object(builder, "drawing_area"));

    gtk_builder_connect_signals(builder,NULL);

    gtk_widget_show(window);

    if ( access("Hantek.cfg", F_OK) ) {
        cfg_fd = open("Hantek.cfg", O_CREAT | O_RDWR, 0666);
        write(cfg_fd, &default_config, sizeof(default_config));
    } else {
        cfg_fd = open("Hantek.cfg", O_RDWR);
    }
    cur_config = mmap(NULL, sizeof(config_t), PROT_READ | PROT_WRITE, MAP_SHARED, cfg_fd, 0);

    //Init all settings
    gtk_button_clicked(GTK_BUTTON(scope_radio));

    gtk_switch_set_state(channel_enable_switch_ch1, cur_config->channel_enable[0]);
    gtk_switch_set_state(channel_enable_switch_ch2, cur_config->channel_enable[1]);

    gtk_combo_box_set_active(channel_coupling_combobox_ch1, cur_config->channel_coupling[0]);
    gtk_combo_box_set_active(channel_coupling_combobox_ch2, cur_config->channel_coupling[1]);

    gtk_combo_box_set_active(channel_probe_combobox_ch1, cur_config->channel_probe[0]);
    gtk_combo_box_set_active(channel_probe_combobox_ch2, cur_config->channel_probe[1]);

    gtk_combo_box_set_active(channel_scale_combobox_ch1, cur_config->channel_scale[0]);
    gtk_combo_box_set_active(channel_scale_combobox_ch2, cur_config->channel_scale[1]);

    gtk_spin_button_set_value(channel_offset_spinbutton_ch1, cur_config->channel_offset[0]);
    gtk_spin_button_set_value(channel_offset_spinbutton_ch2, cur_config->channel_offset[1]);

    gtk_switch_set_state(channel_bwlimit_switch_ch1, cur_config->channel_bwlimit[0]);
    gtk_switch_set_state(channel_bwlimit_switch_ch2, cur_config->channel_bwlimit[1]);

    gtk_combo_box_set_active(time_scale_combobox,     cur_config->time_scale);
    gtk_spin_button_set_value(time_offset_spinbutton, cur_config->time_offset);

    gtk_combo_box_set_active(trigger_source_combobox,   cur_config->trigger_source);
    gtk_combo_box_set_active(trigger_slope_combobox,    cur_config->trigger_slope);
    gtk_combo_box_set_active(trigger_mode_combobox,     cur_config->trigger_mode);
    gtk_spin_button_set_value(trigger_level_spinbutton, cur_config->trigger_level);

    gtk_combo_box_set_active(awg_type_combobox,             cur_config->awg_type);
    gtk_spin_button_set_value(awg_frequency_spinbutton,     cur_config->awg_frequency);
    gtk_spin_button_set_value(awg_amplitude_spinbutton,     cur_config->awg_amplitude);
    gtk_spin_button_set_value(awg_offset_spinbutton,        cur_config->awg_offset);
    gtk_spin_button_set_value(awg_squareduty_spinbutton,    cur_config->awg_squareduty);
    gtk_spin_button_set_value(awg_rampduty_spinbutton,      cur_config->awg_rampduty);
    gtk_spin_button_set_value(awg_trapriseduty_spinbutton,  cur_config->awg_trapriseduty);
    gtk_spin_button_set_value(awg_traphighduty_spinbutton,  cur_config->awg_traphighduty);
    gtk_spin_button_set_value(awg_trapfallduty_spinbutton,  cur_config->awg_trapfallduty);

    gtk_spin_button_set_value(capture_samples_spinbutton,  cur_config->num_samples);

    gtk_main();

    munmap(cur_config, sizeof(config_t));

    close(cfg_fd);

    g_object_unref(builder);

    release_interfaces(device, handle);

    libusb_close(handle);

cleanup:
    libusb_exit(NULL);
    return status;
}

void on_window_main_destroy(GtkWidget *object, gpointer user_data) {
    gtk_main_quit();
}

gboolean on_channel_enable (GtkSwitch* self, gboolean state, gpointer user_data) {
    g_print("%s\n", __func__);
    Hantek_command_t command;

    command.idx     = 0x00;
    command.boh     = 0x0A;
    command.func    = FUNC_SCOPE_SETTING;
    if ( self == channel_enable_switch_ch1 ) {
        command.cmd     = SCOPE_ENABLE_CH1;
        cur_config->channel_enable[0] = state;
    } else if ( self == channel_enable_switch_ch2 ) {
        command.cmd     = SCOPE_ENABLE_CH2;
        cur_config->channel_enable[1] = state;
    } else
        return FALSE;

    command.val[0]  = state;
    command.val[1]  = 0;
    command.val[2]  = 0;
    command.val[3]  = 0;
    command.last    = 0;
    libusb_bulk_transfer(handle, LIBUSB_ENDPOINT_OUT | 2, (unsigned char*)&command, sizeof(command), NULL, 0);

    gtk_switch_set_state(self, state);

    return TRUE;
}

void on_channel_coupling(GtkComboBox *widget, gpointer user_data) {
    g_print("%s\n", __func__);
    Hantek_command_t command;

    int val = atoi(gtk_combo_box_get_active_id(widget));

    command.idx     = 0x00;
    command.boh     = 0x0A;
    command.func    = FUNC_SCOPE_SETTING;
    if ( widget == channel_coupling_combobox_ch1 ) {
        command.cmd     = SCOPE_COUPLING_CH1;
        cur_config->channel_coupling[0] = val;
    } else if ( widget == channel_coupling_combobox_ch2 ) {
        command.cmd     = SCOPE_COUPLING_CH2;
        cur_config->channel_coupling[1] = val;
    } else
        return;

    command.val[0]  = val;
    command.val[1]  = 0;
    command.val[2]  = 0;
    command.val[3]  = 0;
    command.last    = 0;
    libusb_bulk_transfer(handle, LIBUSB_ENDPOINT_OUT | 2, (unsigned char*)&command, sizeof(command), NULL, 0);
}

void on_channel_probe(GtkComboBox *widget, gpointer user_data) {
    g_print("%s\n", __func__);
    Hantek_command_t command;
    GtkComboBox* channel_scale_combobox;
    int probe_val = atoi(gtk_combo_box_get_active_id(widget));

    command.idx     = 0x00;
    command.boh     = 0x0A;
    command.func    = FUNC_SCOPE_SETTING;
    if ( widget == channel_probe_combobox_ch1 ) {
        command.cmd     = SCOPE_PROBEX_CH1;
        channel_scale_combobox = channel_scale_combobox_ch1;
        cur_config->channel_probe[0] = probe_val;
    } else if ( widget == channel_probe_combobox_ch2 ) {
        command.cmd     = SCOPE_PROBEX_CH2;
        channel_scale_combobox = channel_scale_combobox_ch2;
        cur_config->channel_probe[1] = probe_val;
    } else
        return;

    command.val[0]  = probe_val;
    command.val[1]  = 0;
    command.val[2]  = 0;
    command.val[3]  = 0;
    command.last    = 0;
    libusb_bulk_transfer(handle, LIBUSB_ENDPOINT_OUT | 2, (unsigned char*)&command, sizeof(command), NULL, 0);

    int scale_val = gtk_combo_box_get_active(channel_scale_combobox);

    switch (probe_val) {
        case 0: //1X
            gtk_combo_box_set_model(channel_scale_combobox, GTK_TREE_MODEL(liststore_1x));
            break;
        case 1: //10X
            gtk_combo_box_set_model(channel_scale_combobox, GTK_TREE_MODEL(liststore_10x));
            break;
        case 2: //100X
            gtk_combo_box_set_model(channel_scale_combobox, GTK_TREE_MODEL(liststore_100x));
            break;
        case 3: //1000X
            gtk_combo_box_set_model(channel_scale_combobox, GTK_TREE_MODEL(liststore_1000x));
            break;
    }
    gtk_combo_box_set_active(channel_scale_combobox, scale_val);
}

void on_channel_scale(GtkComboBox *widget, gpointer user_data) {
    g_print("%s\n", __func__);
    Hantek_command_t command;
    GtkAdjustment* adj;
    GtkTreeIter active;
    float real_val;
    uint8_t val = gtk_combo_box_get_active(widget);
    int channel;

    gtk_combo_box_get_active_iter(widget, &active);

    gtk_tree_model_get(gtk_combo_box_get_model(widget), &active, 1, &real_val, -1);

    command.idx     = 0x00;
    command.boh     = 0x0A;
    command.func    = FUNC_SCOPE_SETTING;
    if ( widget == channel_scale_combobox_ch1 ) {
        command.cmd     = SCOPE_SCALE_CH1;
        adj = channel_offset_adj_ch1;
        channel = 0;
        cur_config->channel_scale[0] = val;
    } else if ( widget == channel_scale_combobox_ch2 ) {
        command.cmd     = SCOPE_SCALE_CH2;
        adj = channel_offset_adj_ch2;
        channel = 1;
        cur_config->channel_scale[1] = val;
    } else
        return;

    command.val[0]  = val;
    command.val[1]  = 0;
    command.val[2]  = 0;
    command.val[3]  = 0;
    command.last    = 0;
    libusb_bulk_transfer(handle, LIBUSB_ENDPOINT_OUT | 2, (unsigned char*)&command, sizeof(command), NULL, 0);

    gtk_adjustment_set_lower(adj, -4*real_val);
    gtk_adjustment_set_upper(adj, 4*real_val);
    gtk_adjustment_set_step_increment(adj, 8*real_val/200);
    gtk_adjustment_set_page_increment(adj, real_val);

    if ( gtk_combo_box_get_active(trigger_source_combobox) == channel ) {
        gtk_adjustment_set_lower(trigger_level_adj, -4*real_val);
        gtk_adjustment_set_upper(trigger_level_adj, 4*real_val);
        gtk_adjustment_set_step_increment(trigger_level_adj, 8*real_val/200);
        gtk_adjustment_set_page_increment(trigger_level_adj, real_val);
    }
}

void on_channel_offset(GtkSpinButton *spin_button, GtkScrollType scroll, gpointer user_data) {
    g_print("%s\n", __func__);
    Hantek_command_t command;
    GtkComboBox* channel_scale_combobox = NULL;
    GtkAdjustment* channel_offset_adj = NULL;
    double val = gtk_spin_button_get_value(spin_button);

    command.idx     = 0x00;
    command.boh     = 0x0A;
    command.func    = FUNC_SCOPE_SETTING;
    if ( spin_button == channel_offset_spinbutton_ch1 ) {
        command.cmd     = SCOPE_OFFSET_CH1;
        channel_scale_combobox = channel_scale_combobox_ch1;
        channel_offset_adj = channel_offset_adj_ch1;
        cur_config->channel_offset[0] = val;
    } else if ( spin_button == channel_offset_spinbutton_ch2 ) {
        command.cmd     = SCOPE_OFFSET_CH2;
        channel_scale_combobox = channel_scale_combobox_ch2;
        channel_offset_adj = channel_offset_adj_ch2;
        cur_config->channel_offset[1] = val;
    } else {
        return;
    }

    val-=gtk_adjustment_get_lower(channel_offset_adj);
    val*=200;
    val/=gtk_adjustment_get_upper(channel_offset_adj)-gtk_adjustment_get_lower(channel_offset_adj);

    command.val[0]  = (int)val;
    command.val[1]  = 0;
    command.val[2]  = 0;
    command.val[3]  = 0;
    command.last    = 0;
    libusb_bulk_transfer(handle, LIBUSB_ENDPOINT_OUT | 2, (unsigned char*)&command, sizeof(command), NULL, 0);
}

gboolean on_channel_bwlimit(GtkSwitch* self, gboolean state, gpointer user_data) {
    g_print("%s\n", __func__);
    Hantek_command_t command;

    command.idx     = 0x00;
    command.boh     = 0x0A;
    command.func    = FUNC_SCOPE_SETTING;
    if ( self == channel_bwlimit_switch_ch1 ) {
        command.cmd     = SCOPE_BWLIMIT_CH1;
        cur_config->channel_bwlimit[0] = state;
    } else if ( self == channel_bwlimit_switch_ch1 ) {
        command.cmd     = SCOPE_BWLIMIT_CH2;
        cur_config->channel_bwlimit[1] = state;
    } else
        return FALSE;

    command.val[0]  = state;
    command.val[1]  = 0;
    command.val[2]  = 0;
    command.val[3]  = 0;
    command.last    = 0;
    libusb_bulk_transfer(handle, LIBUSB_ENDPOINT_OUT | 2, (unsigned char*)&command, sizeof(command), NULL, 0);

    gtk_switch_set_state(self, state);

    return TRUE;
}


void on_time_scale(GtkComboBox *widget, gpointer user_data) {
    g_print("%s\n", __func__);
    Hantek_command_t command;
    GtkTreeIter active;

    float real_val;
    uint8_t val = gtk_combo_box_get_active(widget);

    gtk_combo_box_get_active_iter(widget, &active);

    gtk_tree_model_get(gtk_combo_box_get_model(widget), &active, 1, &real_val, -1);

    cur_config->time_scale = val;

    command.idx     = 0x00;
    command.boh     = 0x0A;
    command.func    = FUNC_SCOPE_SETTING;
    command.cmd     = SCOPE_SCALE_TIME;
    command.val[0]  = val;
    command.val[1]  = 0;
    command.val[2]  = 0;
    command.val[3]  = 0;
    command.last    = 0;
    libusb_bulk_transfer(handle, LIBUSB_ENDPOINT_OUT | 2, (unsigned char*)&command, sizeof(command), NULL, 0);

    gtk_adjustment_set_lower(time_offset_adj, -15*real_val);
    gtk_adjustment_set_upper(time_offset_adj, 15*real_val);
    gtk_adjustment_set_step_increment(time_offset_adj, real_val/25);
    gtk_adjustment_set_page_increment(time_offset_adj, real_val);
}

void on_time_offset(GtkSpinButton *spin_button, GtkScrollType scroll, gpointer user_data) {
    g_print("%s\n", __func__);
    Hantek_command_t command;
    double val = -gtk_spin_button_get_value(spin_button);

    cur_config->time_offset = -val;

    val-=gtk_adjustment_get_lower(time_offset_adj)/15*6;
    val*=15*2*25;
    val/=gtk_adjustment_get_upper(time_offset_adj)-gtk_adjustment_get_lower(time_offset_adj);

    command.idx     = 0x00;
    command.boh     = 0x0A;
    command.func    = FUNC_SCOPE_SETTING;
    command.cmd     = SCOPE_OFFSET_TIME;
    command.val32   = (int)roundf(val);
    command.size[1] = 0;
    command.last    = 0;
    libusb_bulk_transfer(handle, LIBUSB_ENDPOINT_OUT | 2, (unsigned char*)&command, sizeof(command), NULL, 0);
}

void on_trigger_source (GtkComboBox *widget, gpointer user_data) {
    g_print("%s\n", __func__);
    Hantek_command_t command;
    GtkComboBox* channel_scale_combobox = NULL;
    GtkTreeIter active;
    float real_scale_val;
    int channel = atoi(gtk_combo_box_get_active_id(widget));

    cur_config->trigger_source = channel;

    command.idx     = 0x00;
    command.boh     = 0x0A;
    command.func    = FUNC_SCOPE_SETTING;
    command.cmd     = SCOPE_TRIGGER_SOURCE;
    command.val[0]  = channel;
    command.val[1]  = 0;
    command.val[2]  = 0;
    command.val[3]  = 0;
    command.last    = 0;
    libusb_bulk_transfer(handle, LIBUSB_ENDPOINT_OUT | 2, (unsigned char*)&command, sizeof(command), NULL, 0);

    if ( channel == 0 ) {
        channel_scale_combobox = channel_scale_combobox_ch1;
    } else if ( channel == 1 ) {
        channel_scale_combobox = channel_scale_combobox_ch2;
    } else {
        return;
    }

    gtk_combo_box_get_active_iter(channel_scale_combobox, &active);
    gtk_tree_model_get(gtk_combo_box_get_model(channel_scale_combobox), &active, 1, &real_scale_val, -1);

    gtk_adjustment_set_lower(trigger_level_adj, -4*real_scale_val);
    gtk_adjustment_set_upper(trigger_level_adj, 4*real_scale_val);
    gtk_adjustment_set_step_increment(trigger_level_adj, 8*real_scale_val/200);
    gtk_adjustment_set_page_increment(trigger_level_adj, real_scale_val);
}

void on_trigger_slope (GtkComboBox *widget, gpointer user_data) {
    g_print("%s\n", __func__);
    Hantek_command_t command;
    int val = atoi(gtk_combo_box_get_active_id(widget));

    cur_config->trigger_slope = val;

    command.idx     = 0x00;
    command.boh     = 0x0A;
    command.func    = FUNC_SCOPE_SETTING;
    command.cmd     = SCOPE_TRIGGER_SLOPE;
    command.val[0]  = val;
    command.val[1]  = 0;
    command.val[2]  = 0;
    command.val[3]  = 0;
    command.last    = 0;
    libusb_bulk_transfer(handle, LIBUSB_ENDPOINT_OUT | 2, (unsigned char*)&command, sizeof(command), NULL, 0);
}

void on_trigger_mode (GtkComboBox *widget, gpointer user_data) {
    g_print("%s\n", __func__);
    Hantek_command_t command;
    int val = atoi(gtk_combo_box_get_active_id(widget));

    cur_config->trigger_mode = val;

    command.idx     = 0x00;
    command.boh     = 0x0A;
    command.func    = FUNC_SCOPE_SETTING;
    command.cmd     = SCOPE_TRIGGER_MODE;
    command.val[0]  = val;
    command.val[1]  = 0;
    command.val[2]  = 0;
    command.val[3]  = 0;
    command.last    = 0;
    libusb_bulk_transfer(handle, LIBUSB_ENDPOINT_OUT | 2, (unsigned char*)&command, sizeof(command), NULL, 0);
}

void on_trigger_level(GtkSpinButton *spin_button, GtkScrollType scroll, gpointer user_data) {
    g_print("%s\n", __func__);
    Hantek_command_t command;

    double val = gtk_spin_button_get_value(spin_button);

    cur_config->trigger_level = val;

    val-=gtk_adjustment_get_lower(trigger_level_adj);
    val*=200;
    val/=gtk_adjustment_get_upper(trigger_level_adj)-gtk_adjustment_get_lower(trigger_level_adj);

    command.idx     = 0x00;
    command.boh     = 0x0A;
    command.func    = FUNC_SCOPE_SETTING;
    command.cmd     = SCOPE_TRIGGER_LEVEL;
    command.val[0]  = (int)val;
    command.val[1]  = 0;
    command.val[2]  = 0;
    command.val[3]  = 0;
    command.last    = 0;
    libusb_bulk_transfer(handle, LIBUSB_ENDPOINT_OUT | 2, (unsigned char*)&command, sizeof(command), NULL, 0);
}

void on_start(GtkButton *button, gpointer user_data) {
    g_print("%s\n", __func__);
    Hantek_command_t command;

    command.idx     = 0x00;
    command.boh     = 0x0A;
    command.func    = FUNC_SCOPE_SETTING;
    command.cmd     = SCOPE_START;
    command.val[0]  = 1;
    command.val[1]  = 0;
    command.val[2]  = 0;
    command.val[3]  = 0;
    command.last    = 0;
    libusb_bulk_transfer(handle, LIBUSB_ENDPOINT_OUT | 2, (unsigned char*)&command, sizeof(command), NULL, 0);
}

void on_stop(GtkButton *button, gpointer user_data) {
    g_print("%s\n", __func__);
    Hantek_command_t command;

    command.idx     = 0x00;
    command.boh     = 0x0A;
    command.func    = FUNC_SCOPE_SETTING;
    command.cmd     = SCOPE_START;
    command.val[0]  = 0;
    command.val[1]  = 0;
    command.val[2]  = 0;
    command.val[3]  = 0;
    command.last    = 0;
    libusb_bulk_transfer(handle, LIBUSB_ENDPOINT_OUT | 2, (unsigned char*)&command, sizeof(command), NULL, 0);
}

void on_awg_type(GtkComboBox *widget, gpointer user_data) {
    g_print("%s\n", __func__);
    Hantek_command_t command;
    int val = atoi(gtk_combo_box_get_active_id(widget));

    cur_config->awg_type = val;

    command.idx     = 0x00;
    command.boh     = 0x0A;
    command.func    = FUNC_AWG_SETTING;
    command.cmd     = AWG_TYPE;

    command.val[0]  = val;
    command.val[1]  = 0;
    command.val[2]  = 0;
    command.val[3]  = 0;
    command.last    = 0;
    libusb_bulk_transfer(handle, LIBUSB_ENDPOINT_OUT | 2, (unsigned char*)&command, sizeof(command), NULL, 0);
}

void on_awg_freq(GtkSpinButton *spin_button, GtkScrollType scroll, gpointer user_data) {
    g_print("%s\n", __func__);
    Hantek_command_t command;
    float val = gtk_spin_button_get_value_as_int(spin_button);

    cur_config->awg_frequency = val;

    command.idx     = 0x00;
    command.boh     = 0x0A;
    command.func    = FUNC_AWG_SETTING;
    command.cmd     = AWG_FREQ;
    command.val32   = val;
    command.last    = 0;
    libusb_bulk_transfer(handle, LIBUSB_ENDPOINT_OUT | 2, (unsigned char*)&command, sizeof(command), NULL, 0);
}

void on_awg_amp(GtkSpinButton *spin_button, GtkScrollType scroll, gpointer user_data) {
    g_print("%s\n", __func__);
    Hantek_command_t command;
    float val = gtk_spin_button_get_value(spin_button);

    cur_config->awg_amplitude = val;

    command.idx     = 0x00;
    command.boh     = 0x0A;
    command.func    = FUNC_AWG_SETTING;
    command.cmd     = AWG_AMP;
    command.size[0] = abs((val*1000));
    command.size[1] = (val<0);
    command.last    = 0;
    libusb_bulk_transfer(handle, LIBUSB_ENDPOINT_OUT | 2, (unsigned char*)&command, sizeof(command), NULL, 0);
}

void on_awg_offset(GtkSpinButton *spin_button, GtkScrollType scroll, gpointer user_data) {
    g_print("%s\n", __func__);
    Hantek_command_t command;
    float val = gtk_spin_button_get_value(spin_button);

    cur_config->awg_offset = val;

    command.idx     = 0x00;
    command.boh     = 0x0A;
    command.func    = FUNC_AWG_SETTING;
    command.cmd     = AWG_OFF;
    command.size[0] = abs((val*1000));
    command.size[1] = (val<0);
    command.last    = 0;
    libusb_bulk_transfer(handle, LIBUSB_ENDPOINT_OUT | 2, (unsigned char*)&command, sizeof(command), NULL, 0);
}

void on_awg_square_duty(GtkSpinButton *spin_button, GtkScrollType scroll, gpointer user_data) {
    g_print("%s\n", __func__);
    Hantek_command_t command;
    float val = gtk_spin_button_get_value(spin_button);

    cur_config->awg_squareduty = val;

    command.idx     = 0x00;
    command.boh     = 0x0A;
    command.func    = FUNC_AWG_SETTING;
    command.cmd     = AWG_SQUARE_DUTY;
    command.size[0] = val*100;
    command.size[1] = 0;
    command.last    = 0;
    libusb_bulk_transfer(handle, LIBUSB_ENDPOINT_OUT | 2, (unsigned char*)&command, sizeof(command), NULL, 0);
}

void on_awg_ramp_duty(GtkSpinButton *spin_button, GtkScrollType scroll, gpointer user_data) {
    g_print("%s\n", __func__);
    Hantek_command_t command;
    float val = gtk_spin_button_get_value(spin_button);

    cur_config->awg_rampduty = val;

    command.idx     = 0x00;
    command.boh     = 0x0A;
    command.func    = FUNC_AWG_SETTING;
    command.cmd     = AWG_RAMP_DUTY;
    command.size[0] = val*100;
    command.size[1] = 0;
    command.last    = 0;
    libusb_bulk_transfer(handle, LIBUSB_ENDPOINT_OUT | 2, (unsigned char*)&command, sizeof(command), NULL, 0);
}

void on_awg_trap_duty(GtkSpinButton *spin_button, GtkScrollType scroll, gpointer user_data) {
    g_print("%s\n", __func__);
    Hantek_command_t command;
    float val_rise = gtk_spin_button_get_value(awg_trapriseduty_spinbutton);
    float val_high = gtk_spin_button_get_value(awg_traphighduty_spinbutton);
    float val_fall = gtk_spin_button_get_value(awg_trapfallduty_spinbutton);

    cur_config->awg_trapriseduty = val_rise;
    cur_config->awg_traphighduty = val_high;
    cur_config->awg_trapfallduty = val_fall;

    command.idx     = 0x00;
    command.boh     = 0x0A;
    command.func    = FUNC_AWG_SETTING;
    command.cmd     = AWG_TRAP_DUTY;
    command.val[0]  = val_rise*100;
    command.val[1]  = val_high*100;
    command.val[2]  = val_fall*100;
    command.val[3]  = 0;
    command.last    = 0;
    libusb_bulk_transfer(handle, LIBUSB_ENDPOINT_OUT | 2, (unsigned char*)&command, sizeof(command), NULL, 0);
}

void on_awg_start(GtkButton *button, gpointer user_data) {
    g_print("%s\n", __func__);
    Hantek_command_t command;

    command.idx     = 0x00;
    command.boh     = 0x0A;
    command.func    = FUNC_AWG_SETTING;
    command.cmd     = AWG_START;
    command.val[0]  = 1;
    command.val[1]  = 0;
    command.val[2]  = 0;
    command.val[3]  = 0;
    command.last    = 0;
    libusb_bulk_transfer(handle, LIBUSB_ENDPOINT_OUT | 2, (unsigned char*)&command, sizeof(command), NULL, 0);
}

void on_awg_stop(GtkButton *button, gpointer user_data) {
    g_print("%s\n", __func__);
    Hantek_command_t command;

    command.idx     = 0x00;
    command.boh     = 0x0A;
    command.func    = FUNC_AWG_SETTING;
    command.cmd     = AWG_START;
    command.val[0]  = 0;
    command.val[1]  = 0;
    command.val[2]  = 0;
    command.val[3]  = 0;
    command.last    = 0;
    libusb_bulk_transfer(handle, LIBUSB_ENDPOINT_OUT | 2, (unsigned char*)&command, sizeof(command), NULL, 0);
}

void on_radio(GtkRadioButton *button, gpointer   user_data) {
    g_print("%s\n", __func__);
    Hantek_command_t command;

    command.idx     = 0x00;
    command.boh     = 0x0A;
    command.func    = FUNC_SCREEN_SETTING;
    command.cmd     = 0;
    if ( button == scope_radio )
        command.val[0] = SCREEN_VAL_SCOPE;
    else if ( button == awg_radio )
        command.val[0]= SCREEN_VAL_AWG;
    else if ( button == dmm_radio )
        command.val[0]= SCREEN_VAL_DMM;
    command.val[1]  = 0;
    command.val[2]  = 0;
    command.val[3]  = 0;
    command.last    = 0;
    libusb_bulk_transfer(handle, LIBUSB_ENDPOINT_OUT | 2, (unsigned char*)&command, sizeof(command), NULL, 0);
}

void on_capture_button_clicked(GtkButton *button, gpointer user_data) {
    g_print("%s\n", __func__);
    Hantek_command_t command;
    int num_channels = cur_config->channel_enable[0]+cur_config->channel_enable[1];
    int num_samples  = cur_config->num_samples*num_channels;
    int count = 0;

    while(count < num_samples) {
        int length, actual_length;
        command.idx     = 0;
        command.boh     = 0x0A;
        command.func    = FUNC_SCOPE_CAPTURE;
        command.cmd     = SCOPE_START_RECV;
        command.size[0] = num_samples/2;
        command.size[1] = num_samples/2;
        libusb_bulk_transfer(handle, LIBUSB_ENDPOINT_OUT | 2, (unsigned char*)&command, sizeof(command), NULL, 0);

        length = (num_samples-count)<64?(num_samples-count):64;
        libusb_bulk_transfer(handle, LIBUSB_ENDPOINT_IN | 1, &(capture_buffer[count]), length, &actual_length, 0);
        count+=actual_length;
    }

    gtk_widget_queue_draw(drawing_area);
}

gboolean draw_callback(GtkWidget *widget, cairo_t *cr, gpointer data) {
    g_print("%s\n", __func__);

    int width  = gtk_widget_get_allocated_width(widget);
    int height = gtk_widget_get_allocated_height(widget);
    int num_channels = cur_config->channel_enable[0]+cur_config->channel_enable[1];
    int num_samples  = cur_config->num_samples*num_channels;

    double dashes[] = { 5.0, 5.0 };

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_paint(cr);
    cairo_stroke(cr);

    cairo_set_line_width(cr, 0.5);

    cairo_set_dash(cr, dashes, 2, 0);
    cairo_set_line_width(cr, 0.3);
    cairo_set_source_rgb(cr, 0.9, 0.9, 0.9);
    int num_sector = cur_config->num_samples/100;
    for(int i=1;i<num_sector;i++) {
        cairo_move_to(cr, i*width/num_sector, 0);
        cairo_line_to(cr, i*width/num_sector, height);
    }

    for(int i=1;i<8;i++) {
        cairo_move_to(cr, 0, i*height/8);
        cairo_line_to(cr, width, i*height/8);
    }
    cairo_stroke(cr);

    cairo_set_dash(cr, NULL, 0, 0);
    cairo_set_line_width(cr, 0.5);
    for (int ch=0;ch<2;ch++) {
        if ( cur_config->channel_enable[ch] ) {
            if ( ch == 0 )
                cairo_set_source_rgb(cr, 1, 1, 0);
            else
                cairo_set_source_rgb(cr, 0, 1, 0);

            cairo_move_to(cr, 0, height - ((*((uint8_t(*)[cur_config->num_samples][num_channels])capture_buffer))[0][ch]-29)*height/202);

            for(int x=1;x<cur_config->num_samples;x++) cairo_line_to(cr, x*width/cur_config->num_samples, height - ((*((uint8_t(*)[cur_config->num_samples][num_channels])capture_buffer))[x][ch]-29)*height/202);

            cairo_stroke(cr);
        }
    }

    return FALSE;
}

void on_capture_samples(GtkSpinButton *spin_button, GtkScrollType scroll, gpointer user_data) {
    cur_config->num_samples = gtk_spin_button_get_value_as_int(spin_button);
}
