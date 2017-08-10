SANCUS_SUPPORT_DIR = /usr/local/share/sancus-support
VENDOR_KEY         = 4078d505d82099ba
# add yourself to 'dialout' group for r/w permissions on /dev/ttyUSB*
# use 'dmesg | tail' to discover recently attached ttyUSB* devices
DEVICE             = /dev/ttyUSB0

CC                 = sancus-cc
LD                 = sancus-ld
CRYPTO             = sancus-crypto
LOAD               = sancus-loader
RM                 = rm -f

CFLAGS             = -I$(SANCUS_SUPPORT_DIR)/include/ -Wall --verbose #-o0 -g
ifdef __SANCUS_SIM
    CFLAGS += -D__SANCUS_SIM
endif

MEM_SIZE_FLAGS      = --ram-size 16K --rom-size 41K
SIM_FLAGS           = $(MEM_SIZE_FLAGS)

LDFLAGS            = --standalone $(MEM_SIZE_FLAGS) --verbose --verbose
LIBS               = -L$(SANCUS_SUPPORT_DIR)/lib -ldev-uart
CRYPTOFLAGS        = --key $(VENDOR_KEY) --debug --fill-macs
LOADFLAGS          = -device $(DEVICE) -baudrate 115200

NAME               = main
OBJECTS            = main.o sm_io_wrap.o
TARGET             = $(NAME).elf
TARGET_NO_MACS     = $(TARGET)-no-macs

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

$(TARGET): $(TARGET_NO_MACS)
	$(CRYPTO) $(CRYPTOFLAGS) -o $@ $<

$(TARGET_NO_MACS): $(OBJECTS)
	@echo "========== NOW LINKING =========="
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)

.PHONY: load
load: $(TARGET)
	$(MAKE) --always-make
	$(LOAD) $(LOADFLAGS) $<

.PHONY: debug
debug: $(TARGET)
	$(MAKE) --always-make
	sancus-minidebug

.PHONY: sim
sim:
	$(MAKE) CFLAGS="$(CFLAGS) -D__SANCUS_SIM" --always-make
	sancus-sim $(SIM_FLAGS) $(TARGET)


.PHONY: clean
clean:
	$(RM) $(TARGET) $(TARGET_NO_MACS) $(OBJECTS)
	$(RM) *.fst *.bin
