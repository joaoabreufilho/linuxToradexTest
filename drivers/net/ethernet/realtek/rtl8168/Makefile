################################################################################
#
# r8168 is the Linux device driver released for Realtek Gigabit Ethernet
# controllers with PCI-Express interface.
#
# Copyright(c) 2018 Realtek Semiconductor Corp. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the Free
# Software Foundation; either version 2 of the License, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
# more details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, see <http://www.gnu.org/licenses/>.
#
# Author:
# Realtek NIC software team <nicfae@realtek.com>
# No. 2, Innovation Road II, Hsinchu Science Park, Hsinchu 300, Taiwan
#
################################################################################

################################################################################
#  This product is covered by one or more of the following patents:
#  US6,570,884, US6,115,776, and US6,327,625.
################################################################################

CONFIG_SOC_LAN = n
ENABLE_FIBER_SUPPORT = n
ENABLE_REALWOW_SUPPORT = n
ENABLE_DASH_SUPPORT = n
ENABLE_DASH_PRINTER_SUPPORT = n
CONFIG_DOWN_SPEED_100 = n
CONFIG_ASPM = y
ENABLE_S5WOL = y
ENABLE_S5_KEEP_CURR_MAC = n
ENABLE_EEE = n
ENABLE_S0_MAGIC_PACKET = n

obj-$(CONFIG_R8168) += r8168.o
r8168-objs := r8168_n.o r8168_asf.o rtl_eeprom.o rtltool.o

EXTRA_CFLAGS += -DCONFIG_ASPM
EXTRA_CFLAGS += -DENABLE_S5WOL
