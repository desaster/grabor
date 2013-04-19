#=============================================================================#
# ARM makefile
#
# author: Freddie Chopin, http://www.freddiechopin.info/
# last change: 2012-04-07
#
# this makefile is based strongly on many examples found in the network
#
# Adopted for the grabor project by Upi Tamminen
#=============================================================================#

#=============================================================================#
# toolchain configuration
#=============================================================================#

TOOLCHAIN = arm-none-eabi-

CXX = $(TOOLCHAIN)g++
CC = $(TOOLCHAIN)gcc
AS = $(TOOLCHAIN)gcc -x assembler-with-cpp
OBJCOPY = $(TOOLCHAIN)objcopy
OBJDUMP = $(TOOLCHAIN)objdump
SIZE = $(TOOLCHAIN)size
RM = rm -f

#=============================================================================#
# project configuration
#=============================================================================#

# project name
PROJECT = grabor

# core type
CORE = cortex-m3

# linker script
LD_SCRIPT = LPC1758_59_67_68_69_rom.ld

# output folder (absolute or relative path, leave empty for in-tree compilation)
OUT_DIR = out

# lwIP
DIR_LWIP = lwip-1.4.1/src

# global definitions for C++, C and ASM (e.g. "symbol_with_value=0xDEAD symbol_without_value")
GLOBAL_DEFS =

# C++ definitions
CXX_DEFS =

# C definitions
C_DEFS =

# ASM definitions
AS_DEFS =

# include directories (absolute or relative paths to additional folders with
# headers, current folder is always included)
INC_DIRS = inc
INC_DIRS += $(DIR_LWIP)/include $(DIR_LWIP)/include/ipv4

# library directories (absolute or relative paths to additional folders with
# libraries)
LIB_DIRS =

# libraries (additional libraries for linking, e.g. "-lm -lsome_name" to link
# math library libm.a and libsome_name.a)
LIBS =

# additional directories with source files (absolute or relative paths to
# folders with source files, current folder is always included)
SRCS_DIRS = src
SRCS_DIRS += $(DIR_LWIP)/core/ $(DIR_LWIP)/core/ipv4/ $(DIR_LWIP)/netif/
SRCS_DIRS += $(DIR_LWIP)/api/

# extension of C++ files
CXX_EXT = cpp

# wildcard for C++ source files (all files with CXX_EXT extension found in
# current folder and SRCS_DIRS folders will be compiled and linked)
CXX_SRCS = $(wildcard $(patsubst %, %/*.$(CXX_EXT), . $(SRCS_DIRS)))

# extension of C files
C_EXT = c

# wildcard for C source files (all files with C_EXT extension found in current
# folder and SRCS_DIRS folders will be compiled and linked)
#C_SRCS = $(wildcard $(patsubst %, %/*.$(C_EXT), . $(SRCS_DIRS)))
C_SRCS =

C_SRCS += cpu.c
C_SRCS += dma.c
C_SRCS += gpio.c
C_SRCS += i2c.c
C_SRCS += led.c
C_SRCS += main.c
C_SRCS += spi.c
C_SRCS += tvp7002.c
C_SRCS += uart0.c
C_SRCS += vectors.c
C_SRCS += utils.c
C_SRCS += cpld.c
C_SRCS += tprintf.c

C_SRCS += enc28j60.c
C_SRCS += enc28j60if.c
C_SRCS += httpd.c

# lwip (the full paths are actually ignored here, since this Makefile is funny)
C_SRCS += $(LWIPDIR)/core/init.c
C_SRCS += $(LWIPDIR)/core/mem.c
C_SRCS += $(LWIPDIR)/core/memp.c
C_SRCS += $(LWIPDIR)/core/netif.c
C_SRCS += $(LWIPDIR)/core/tcp.c
C_SRCS += $(LWIPDIR)/core/stats.c
C_SRCS += $(LWIPDIR)/core/tcp_out.c
C_SRCS += $(LWIPDIR)/core/tcp_in.c
C_SRCS += $(LWIPDIR)/core/pbuf.c
C_SRCS += $(LWIPDIR)/core/timers.c
C_SRCS += $(LWIPDIR)/core/def.c
C_SRCS += $(LWIPDIR)/core/raw.c
C_SRCS += $(LWIPDIR)/core/udp.c
C_SRCS += $(LWIPDIR)/core/ipv4/ip.c
C_SRCS += $(LWIPDIR)/core/ipv4/inet_chksum.c
C_SRCS += $(LWIPDIR)/core/ipv4/ip_addr.c
C_SRCS += $(LWIPDIR)/core/ipv4/ip_frag.c
C_SRCS += $(LWIPDIR)/core/ipv4/icmp.c
C_SRCS += $(LWIPDIR)/netif/ethernetif.c
C_SRCS += $(LWIPDIR)/netif/etharp.c
C_SRCS += $(LWIPDIR)/api/err.c

# extension of ASM files
AS_EXT = S

# wildcard for ASM source files (all files with AS_EXT extension found in
# current folder and SRCS_DIRS folders will be compiled and linked)
AS_SRCS = $(wildcard $(patsubst %, %/*.$(AS_EXT), . $(SRCS_DIRS)))

# optimization flags ("-O0" - no optimization, "-O1" - optimize, "-O2" -
# optimize even more, "-Os" - optimize for size or "-O3" - optimize yet more) 
OPTIMIZATION = -Os

# set to 1 to optimize size by removing unused code and data during link phase
REMOVE_UNUSED = 1

# set to 1 to compile and link additional code required for C++
USES_CXX = 0

# define warning options here
CXX_WARNINGS = -Wall -Wextra
C_WARNINGS = -Wall -Wstrict-prototypes -Wextra

# C++ language standard ("c++98", "gnu++98" - default, "c++0x", "gnu++0x")
CXX_STD = gnu++98

# C language standard ("c89" / "iso9899:1990", "iso9899:199409",
# "c99" / "iso9899:1999", "gnu89" - default, "gnu99")
C_STD = gnu89

#=============================================================================#
# set the VPATH according to SRCS_DIRS
#=============================================================================#

VPATH = $(SRCS_DIRS)

#=============================================================================#
# when using output folder, append trailing slash to its name
#=============================================================================#

ifeq ($(strip $(OUT_DIR)), )
	OUT_DIR_F =
else
	OUT_DIR_F = $(strip $(OUT_DIR))/
endif

#=============================================================================#
# various compilation flags
#=============================================================================#

# core flags
CORE_FLAGS = -mcpu=$(CORE) -mthumb

# flags for C++ compiler
CXX_FLAGS = -std=$(CXX_STD) -g -ggdb3 -fno-rtti -fno-exceptions -fverbose-asm -Wa,-ahlms=$(OUT_DIR_F)$(notdir $(<:.$(CXX_EXT)=.lst))

# flags for C compiler
C_FLAGS = -std=$(C_STD) -g -ggdb3 -fverbose-asm -Wa,-ahlms=$(OUT_DIR_F)$(notdir $(<:.$(C_EXT)=.lst))

# flags for assembler
AS_FLAGS = -g -ggdb3 -Wa,-amhls=$(OUT_DIR_F)$(notdir $(<:.$(AS_EXT)=.lst))

# flags for linker
LD_FLAGS = -T$(LD_SCRIPT) -g -Wl,-Map=$(OUT_DIR_F)$(PROJECT).map,--cref,--no-warn-mismatch

# process option for removing unused code
ifeq ($(REMOVE_UNUSED), 1)
	LD_FLAGS += -Wl,--gc-sections
	OPTIMIZATION += -ffunction-sections -fdata-sections
endif

# if __USES_CXX is defined for ASM then code for global/static constructors /
# destructors is compiled; if -nostartfiles option for linker is added then C++
# initialization / finalization code is not linked
ifeq ($(USES_CXX), 1)
	AS_DEFS += __USES_CXX
else
	LD_FLAGS += -nostartfiles
endif

#=============================================================================#
# do some formatting
#=============================================================================#

CXX_OBJS = $(addprefix $(OUT_DIR_F), $(notdir $(CXX_SRCS:.$(CXX_EXT)=.o)))
C_OBJS = $(addprefix $(OUT_DIR_F), $(notdir $(C_SRCS:.$(C_EXT)=.o)))
AS_OBJS = $(addprefix $(OUT_DIR_F), $(notdir $(AS_SRCS:.$(AS_EXT)=.o)))
OBJS = $(AS_OBJS) $(C_OBJS) $(CXX_OBJS) $(USER_OBJS)
DEPS = $(OBJS:.o=.d)
INC_DIRS_F = -I. $(patsubst %, -I%, $(INC_DIRS))
LIB_DIRS_F = $(patsubst %, -L%, $(LIB_DIRS))
GLOBAL_DEFS_F = $(patsubst %, -D%, $(GLOBAL_DEFS))
CXX_DEFS_F = $(patsubst %, -D%, $(CXX_DEFS))
C_DEFS_F = $(patsubst %, -D%, $(C_DEFS))
AS_DEFS_F = $(patsubst %, -D%, $(AS_DEFS))

ELF = $(OUT_DIR_F)$(PROJECT).elf
HEX = $(OUT_DIR_F)$(PROJECT).hex
BIN = $(OUT_DIR_F)$(PROJECT).bin
LSS = $(OUT_DIR_F)$(PROJECT).lss
DMP = $(OUT_DIR_F)$(PROJECT).dmp

# format final flags for tools, request dependancies for C++, C and asm
CXX_FLAGS_F = $(CORE_FLAGS) $(OPTIMIZATION) $(CXX_WARNINGS) $(CXX_FLAGS) $(GLOBAL_DEFS_F) $(CXX_DEFS_F) -MD -MP -MF $(OUT_DIR_F)$(@F:.o=.d) $(INC_DIRS_F)
C_FLAGS_F = $(CORE_FLAGS) $(OPTIMIZATION) $(C_WARNINGS) $(C_FLAGS) $(GLOBAL_DEFS_F) $(C_DEFS_F) -MD -MP -MF $(OUT_DIR_F)$(@F:.o=.d) $(INC_DIRS_F)
AS_FLAGS_F = $(CORE_FLAGS) $(AS_FLAGS) $(GLOBAL_DEFS_F) $(AS_DEFS_F) -MD -MP -MF $(OUT_DIR_F)$(@F:.o=.d) $(INC_DIRS_F)
LD_FLAGS_F = $(CORE_FLAGS) $(LD_FLAGS) $(LIB_DIRS_F)

#contents of output directory
GENERATED = $(wildcard $(patsubst %, $(OUT_DIR_F)*.%, bin d dmp elf hex lss lst map o))

#=============================================================================#
# make all
#=============================================================================#

all : make_output_dir $(ELF) $(LSS) $(DMP) $(HEX) $(BIN) print_size

# make object files dependent on Makefile
$(OBJS) : Makefile
# make .elf file dependent on linker script
$(ELF) : $(LD_SCRIPT)

#-----------------------------------------------------------------------------#
# linking - objects -> elf
#-----------------------------------------------------------------------------#

$(ELF) : $(OBJS)
	@echo 'Linking target: $(ELF)'
	$(CXX) $(LD_FLAGS_F) $(OBJS) $(LIBS) -o $@
	@echo ' '

#-----------------------------------------------------------------------------#
# compiling - C++ source -> objects
#-----------------------------------------------------------------------------#

$(OUT_DIR_F)%.o : %.$(CXX_EXT)
	@echo 'Compiling file: $<'
	$(CXX) -c $(CXX_FLAGS_F) $< -o $@
	@echo ' '

#-----------------------------------------------------------------------------#
# compiling - C source -> objects
#-----------------------------------------------------------------------------#

$(OUT_DIR_F)%.o : %.$(C_EXT)
	@echo 'Compiling file: $<'
	$(CC) -c $(C_FLAGS_F) $< -o $@
	@echo ' '

#-----------------------------------------------------------------------------#
# assembling - ASM source -> objects
#-----------------------------------------------------------------------------#

$(OUT_DIR_F)%.o : %.$(AS_EXT)
	@echo 'Assembling file: $<'
	$(AS) -c $(AS_FLAGS_F) $< -o $@
	@echo ' '

#-----------------------------------------------------------------------------#
# memory images - elf -> hex, elf -> bin
#-----------------------------------------------------------------------------#

$(HEX) : $(ELF)
	@echo 'Creating IHEX image: $(HEX)'
	$(OBJCOPY) -O ihex $< $@
	@echo ' '

$(BIN) : $(ELF)
	@echo 'Creating binary image: $(BIN)'
	$(OBJCOPY) -O binary $< $@
	@echo ' '

#-----------------------------------------------------------------------------#
# memory dump - elf -> dmp
#-----------------------------------------------------------------------------#

$(DMP) : $(ELF)
	@echo 'Creating memory dump: $(DMP)'
	$(OBJDUMP) -x --syms $< > $@
	@echo ' '

#-----------------------------------------------------------------------------#
# extended listing - elf -> lss
#-----------------------------------------------------------------------------#

$(LSS) : $(ELF)
	@echo 'Creating extended listing: $(LSS)'
	$(OBJDUMP) -S $< > $@
	@echo ' '

#-----------------------------------------------------------------------------#
# print the size of the objects and the .elf file
#-----------------------------------------------------------------------------#

print_size :
	@echo 'Size of modules:'
	$(SIZE) -B -t --common $(OBJS) $(USER_OBJS)
	@echo ' '
	@echo 'Size of target .elf file:'
	$(SIZE) -B $(ELF)
	@echo ' '

#-----------------------------------------------------------------------------#
# create the desired output directory
#-----------------------------------------------------------------------------#

make_output_dir :
	$(shell mkdir $(OUT_DIR_F) 2>/dev/null)

#=============================================================================#
# make clean
#=============================================================================#

clean:
ifeq ($(strip $(OUT_DIR_F)), )
	@echo 'Removing all generated output files'
else
	@echo 'Removing all generated output files from output directory: $(OUT_DIR_F)'
endif
ifneq ($(strip $(GENERATED)), )
	$(RM) $(GENERATED)
else
	@echo 'Nothing to remove...'
endif

#=============================================================================#
# global exports
#=============================================================================#

.PHONY: all clean dependents

.SECONDARY:

# include dependancy files
-include $(DEPS)
