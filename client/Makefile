#---------------------------------------------------------------------------------
# NDS Store Makefile - Uniwersalny
#---------------------------------------------------------------------------------

# Proba automatycznego znalezienia DEVKITARM jesli nie jest ustawione
ifeq ($(strip $(DEVKITPRO)),)
	$(error "Prose ustawic DEVKITPRO w srodowisku")
endif

include $(DEVKITPRO)/devkitARM/nds_rules

TARGET		:=	NDSStore
SOURCES		:=	source
DATA		:=	data
INCLUDES	:=	include
BUILD		:=	build
export OUTPUT	:=	$(CURDIR)

ARCH	:=	-mthumb -mthumb-interwork
CFLAGS	:=	$(ARCH) -O2 -Wall -fomit-frame-pointer -ffast-math \
			$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			-DARM9
ASFLAGS	:=	$(ARCH) -march=armv5te -mtune=arm946e-s
LDFLAGS	:=	$(ARCH) -Wl,-Map,$(notdir $*.map)

LIBS	:=	-lnds9 -ldswifi9 -lfat

#---------------------------------------------------------------------------------
export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir))
export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))

export LD	:=	$(CXX)
export OFILES	:=	$(SFILES:.s=.o) $(CFILES:.c=.o) $(CPPFILES:.cpp=.o)

.PHONY: all clean

all: $(BUILD) $(TARGET).nds

$(BUILD):
	@mkdir -p $@

$(TARGET).nds: $(TARGET).arm9
	@ndstool -c $@ -9 $<

$(TARGET).arm9: $(TARGET).elf
	@$(OBJCOPY) -O binary $< $@

$(TARGET).elf: $(OFILES)
	@$(LD) $(LDFLAGS) $(OFILES) $(LIBS) -o $@

clean:
	@rm -rf $(BUILD) $(TARGET).nds $(TARGET).elf $(TARGET).arm9

# Pattern rules
%.o: %.cpp
	@echo $(notdir $<)
	@$(CXX) -MMD -MP -MF $(DEPSDIR)/$*.d $(CFLAGS) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	@echo $(notdir $<)
	@$(CC) -MMD -MP -MF $(DEPSDIR)/$*.d $(CFLAGS) -c $< -o $@

-include $(DEPSDIR)/*.d
