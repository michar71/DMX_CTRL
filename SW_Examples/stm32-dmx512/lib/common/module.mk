# module.mk
# 
# STM32 DMX512
# Copyright (C) 2012 Erik Van Hamme, all rights reserved
# 
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 3 of the License, or (at your option) any later version.
# 
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
# 

# This file is used for configuration of the build system.

SRCS += \
	common/src/circularbuffer_u8.c \
	common/src/cppdeclarations.cpp \
	common/src/hw_crc32.c \
	common/src/hw_usart.c \
	common/src/packetizer.c \
	common/src/syscalls.c

HDRS += \
	common/src/circularbuffer_u8.h \
	common/src/hw_crc32.h \
	common/src/hw_usart.h \
	common/src/packetizer.h

