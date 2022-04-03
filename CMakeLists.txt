#    Hantek 2D72 handheld oscillosope tool for linux
#    Copyright (C) 2021 Luca Oliva
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <https://www.gnu.org/licenses/>.

project(Hantek)
cmake_minimum_required(VERSION 3.18)

find_package(PkgConfig)

add_link_options(-rdynamic)

pkg_search_module(LIBUSB REQUIRED libusb-1.0)
pkg_search_module(LIBGTK REQUIRED gtk+-3.0)

include_directories(${LIBUSB_INCLUDE_DIRS} ${LIBGTK_INCLUDE_DIRS})

add_custom_command(OUTPUT Hantek.glade COMMAND cp ${CMAKE_SOURCE_DIR}/Hantek.glade . DEPENDS ${CMAKE_SOURCE_DIR}/Hantek.glade)
add_custom_target(Hantek_glade ALL DEPENDS Hantek.glade)

add_executable(Hantek Hantek.c)
target_link_libraries(Hantek ${LIBUSB_LINK_LIBRARIES} ${LIBGTK_LINK_LIBRARIES} m)
