# Arduino makefile
#
# This makefile allows you to build sketches from the command line
# without the Arduino environment (or Java).
#
# The Arduino environment does preliminary processing on a sketch before
# compiling it.  If you're using this makefile instead, you'll need to do
# a few things differently:
#
#   - Give your program's file a .cpp extension (e.g. foo.cpp).
#
#   - Put this line at top of your code: #include <WProgram.h>
#
#   - Write prototypes for all your functions (or define them before you
#     call them).  A prototype declares the types of parameters a
#     function will take and what type of value it will return.  This
#     means that you can have a call to a function before the definition
#     of the function.  A function prototype looks like the first line of
#     the function, with a semi-colon at the end.  For example:
#     int digitalRead(int pin);
#
# Instructions for using the makefile:
#
#  1. Copy this file into the folder with your sketch.
#
#  2. Below, modify the line containing "TARGET" to refer to the name of
#     of your program's file without an extension (e.g. TARGET = foo).
#
#  3. Modify the line containg "ARDUINO" to point the directory that
#     contains the Arduino core (for normal Arduino installations, this
#     is the lib/targets/arduino sub-directory).
#
#  4. Modify the line containing "PORT" to refer to the filename
#     representing the USB or serial connection to your Arduino board
#     (e.g. PORT = /dev/tty.USB0).  If the exact name of this file
#     changes, you can use * as a wildcard (e.g. PORT = /dev/tty.USB*).
#
#  5. At the command line, change to the directory containing your
#     program's file and the makefile.
#
#  6. Type "make" and press enter to compile/verify your program.
#
#  7. Type "make deploy", reset your Arduino board, and press enter  to
#     upload your program to the Arduino board.
#
# $Id$

LIBS = Wire MPU6050 I2Cdev Servo EEPROM
LIBS_INC = $(LIBS) Wire/utility

LIB_DIRS = $(patsubst %,lib/%,$(LIBS))
LIB_INC = $(patsubst %,-Ilib/%,$(LIBS_INC))
LIB_SRC = $(join $(LIBS:%=lib/%), $(LIBS:%=/%.cpp)) lib/Wire/utility/twi.c

ARDUINO_C = avr-libc/malloc.c avr-libc/realloc.c WInterrupts.c wiring.c \
			wiring_digital.c wiring_pulse.c
			#wiring_analog.c
			#wiring_shift.c
ARDUINO_CXX = CDC.cpp HardwareSerial.cpp \
			  new.cpp Print.cpp Stream.cpp \
			  WMath.cpp WString.cpp
			  # HID.cpp
			  # USBCore.cpp
			  # Tone.cpp
			  # IPAddress.cpp \
				# main.cpp not included

#PORT = /dev/tty.usbserial*
PORT = COM5
TARGET = flightcontrol
ARDUINO = arduino/cores/arduino
ARDUINO_H = $(ARDUINO)/arduino.h
# SRC = $(ARDUINO)/buffer.c $(ARDUINO)/pins_arduino.c \
# $(ARDUINO)/Serial.c $(ARDUINO)/uart.c $(ARDUINO)/wiring.c
SRC = $(ARDUINO_C:%=$(ARDUINO)/%) $(filter %.c, $(LIB_SRC))
CXXSRC = $(ARDUINO_CXX:%=$(ARDUINO)/%) $(wildcard src/*.cpp) $(filter %.cpp, $(LIB_SRC))
INC=-Iinclude -Iarduino/cores/arduino -Iarduino/variants/standard $(LIB_INC)
MCU = atmega328p
F_CPU = 16000000
FORMAT = ihex
UPLOAD_RATE = 115200

# Name of this Makefile (used for "make depend").
MAKEFILE = Makefile

# Debugging format.
# Native formats for AVR-GCC's -g are stabs [default], or dwarf-2.
# AVR (extended) COFF requires stabs, plus an avr-objcopy run.
DEBUG = stabs

OPT = s

# Place -D or -U options here
CDEFS = -DF_CPU=$(F_CPU) -DARDUINO=105 -DUSB_VID=null -DUSB_PID=null
CXXDEFS = -DF_CPU=$(F_CPU) -DARDUINO=105 -DUSB_VID=null -DUSB_PID=null

# Place -I options here
CINCS = -I$(ARDUINO)
CXXINCS = -I$(ARDUINO)

# Compiler flag to set the C Standard level.
# c89   - "ANSI" C
# gnu89 - c89 plus GCC extensions
# c99   - ISO C99 standard (not yet fully implemented)
# gnu99 - c99 plus GCC extensions
CSTANDARD = -std=gnu99
CDEBUG = -g$(DEBUG)
CWARN = -Wall -Wstrict-prototypes -fno-exceptions
CTUNING = -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
#CEXTRA = -Wa,-adhlns=$(<:.c=.lst)

CXXWARN = -Wall -fno-exceptions

COMMONFLAGS = -fdata-sections -ffunction-sections -MMD
CFLAGS = $(CDEBUG) $(CDEFS) $(CINCS) -O$(OPT)  $(CSTANDARD) $(CEXTRA) $(INC) $(COMMONFLAGS)
CXXFLAGS = $(CDEFS) $(CINCS) -O$(OPT) $(INC) $(COMMONFLAGS)
#ASFLAGS = -Wa,-adhlns=$(<:.S=.lst),-gstabs 
LDFLAGS = -Wl,--gc-sections -lm


# Programming support using avrdude. Settings and variables.
AVRDUDE_PROGRAMMER = arduino
AVRDUDE_PORT = $(PORT)
AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex:i
AVRDUDE_FLAGS = -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER) \
  -b $(UPLOAD_RATE)

# Program settings
CC = avr-gcc
CXX = avr-g++
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
NM = avr-nm
AVRDUDE = avrdude
REMOVE = rm -f
MV = mv -f
ifeq ($(OS),Windows_NT)
	MKDIR = -$(shell cmd.exe /c mkdir $(subst /,\,$(1)) > NUL 2>&1)
else
	MKDIR = mkdir -p $(1)
endif

# Define all object files.
OBJ_DIR = obj/
OBJ = $(SRC:%.c=$(OBJ_DIR)%.o) $(CXXSRC:%.cpp=$(OBJ_DIR)%.o)

# Define all listing files.
LST = $(ASRC:.S=.lst) $(CXXSRC:.cpp=.lst) $(SRC:.c=.lst)

# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS = -mmcu=$(MCU) -I. $(CFLAGS)
ALL_CXXFLAGS = -mmcu=$(MCU) -I. $(CXXFLAGS)
ALL_ASFLAGS = -mmcu=$(MCU) -I. -x assembler-with-cpp $(ASFLAGS)

# Default target.
all: build

build: elf hex eep
	$(SIZE) $(TARGET).hex

rebuild: clean build

elf: $(TARGET).elf
hex: $(TARGET).hex
eep: $(TARGET).eep
lss: $(TARGET).lss 
sym: $(TARGET).sym

# Program the device.  
deploy: $(TARGET).hex $(TARGET).eep
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH)




# Convert ELF to COFF for use in debugging / simulating in AVR Studio or VMLAB.
COFFCONVERT=$(OBJCOPY) --debugging \
--change-section-address .data-0x800000 \
--change-section-address .bss-0x800000 \
--change-section-address .noinit-0x800000 \
--change-section-address .eeprom-0x810000 


coff: $(TARGET).elf
	$(COFFCONVERT) -O coff-avr $(TARGET).elf $(TARGET).cof


extcoff: $(TARGET).elf
	$(COFFCONVERT) -O coff-ext-avr $(TARGET).elf $(TARGET).cof


.SUFFIXES: .elf .hex .eep .lss .sym .o

.elf.hex:
	@$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

.elf.eep:
	@-$(OBJCOPY) -O $(FORMAT) -j .eeprom --set-section-flags=.eeprom=alloc,load \
	--no-change-warnings --change-section-lma .eeprom=0 $< $@

# Create extended listing file from ELF output file.
.elf.lss:
	$(OBJDUMP) -h -S $< > $@

# Create a symbol table from ELF output file.
.elf.sym:
	$(NM) -n $< > $@



# Link: create ELF output file from object files.
$(TARGET).elf: $(OBJ)
	@$(CC) $(ALL_CFLAGS) $(OBJ) --output $@ $(LDFLAGS)


# Compile: create object files from C++ source files.
$(OBJ_DIR)src/%.o: src/%.cpp
	$(call MKDIR,$(dir $@))
	@$(CXX) -c $(ALL_CXXFLAGS) $(CXXWARN) $< -o $@

# Compile: create object files from C++ source files.
$(OBJ_DIR)%.o: %.cpp
	$(call MKDIR,$(dir $@))
	@$(CXX) -c $(ALL_CXXFLAGS) -w $< -o $@

# Compile: create object files from C source files.
$(OBJ_DIR)%.o: %.c
	$(call MKDIR,$(dir $@))
	@$(CC) -c $(ALL_CFLAGS) $< -o $@ 

# Compile: create assembler files from C source files.
.c.s:
	$(CC) -S $(ALL_CFLAGS) $< -o $@


# Assemble: create object files from assembler source files.
.S.o:
	$(CC) -c $(ALL_ASFLAGS) $< -o $@



# Target: clean project.
clean:
	@$(REMOVE) $(OBJ) $(OBJ:.o=.d)

depend:
	if grep '^# DO NOT DELETE' $(MAKEFILE) >/dev/null; \
	then \
		sed -e '/^# DO NOT DELETE/,$$d' $(MAKEFILE) > \
			$(MAKEFILE).$$$$ && \
		$(MV) $(MAKEFILE).$$$$ $(MAKEFILE); \
	fi
	echo '# DO NOT DELETE THIS LINE -- make depend depends on it.' \
		>> $(MAKEFILE); \
	$(CC) -M -mmcu=$(MCU) $(CDEFS) $(CINCS) $(SRC) $(ASRC) >> $(MAKEFILE)

.PHONY:	all build elf hex eep lss sym program coff extcoff clean depend

-include $(OBJ:.o=.d)

