VENDOR_KEY         = 4078d505d82099ba
ROM                = 32K
RAM                = 24K

CC                 = sancus-cc
LD                 = sancus-ld
SIM                = sancus-sim
CRYPTO             = sancus-crypto
RM                 = rm -f

CFLAGS             = -g -Wall
LDFLAGS            = --standalone --rom-size $(ROM) --ram-size $(RAM)
LIBS               =
SIMFLAGS           = --rom-size $(ROM) --ram-size $(RAM)

ODIR		   = obj
EDIR		   = executables

OBJECTS            = $(ODIR)/$(NAME).o
TARGET             = $(EDIR)/$(NAME).elf
TARGET_NO_MACS     = $(EDIR)/no-macs-$(NAME).elf

.PHONY: all
all: $(TARGET)

$(ODIR)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET_NO_MACS): $(OBJECTS) 					
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)

$(TARGET): $(TARGET_NO_MACS)
	$(CRYPTO) --key $(VENDOR_KEY) --fill-macs -o $@ $<

.PHONY: sim
sim: $(TARGET)
	$(SIM) $(SIMFLAGS) $<

.PHONY: clean
clean:
	$(RM) obj/* executables/* *.o *.elf *.bin *.fst


