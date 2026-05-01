#---------------------------------------------------------------------------------
# NDS Store Makefile
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/nds_rules

# Target name
TARGET		:=	NDSStore
# Source directories
SOURCES		:=	source
# Data directories (for binary files like graphics)
DATA		:=	data
# Include directories
INCLUDES	:=	include

# Build directory
BUILD		:=	build
# Output directory
export OUTPUT	:=	$(CURDIR)

# Compilation flags
ARCH	:=	-mthumb -mthumb-interwork
CFLAGS	:=	$(ARCH) -O2 -Wall -fomit-frame-pointer -ffast-math \
			$(include $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir))) \
			-DARM9
ASFLAGS	:=	$(ARCH) -march=armv5te -mtune=arm946e-s
LDFLAGS	:=	$(ARCH) -Wl,-Map,$(notdir $*.map)

# Libraries
LIBS	:=	-lnds9 -ldswifi9 -lfat

#---------------------------------------------------------------------------------
# Build rules
#---------------------------------------------------------------------------------
export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir))
export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

export OFILES	:=	$(BINFILES:.bin=.o) $(SFILES:.s=.o) $(CFILES:.c=.o) $(CPPFILES:.cpp=.o)
export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

.PHONY: all clean

all: $(BUILD) $(TARGET).nds

$(BUILD):
	@mkdir -p $@

$(TARGET).nds: $(TARGET).arm9
	@ndstool -c $@ -9 $<

$(TARGET).arm9: $(TARGET).elf
	@$(OBJCOPY) -O binary $< $@

$(TARGET).elf: $(OFILES)
	@$(LD) $(LDFLAGS) $(OFILES) $(LIBPATHS) $(LIBS) -o $@

clean:
	@rm -rf $(BUILD) $(TARGET).nds $(TARGET).elf $(TARGET).arm9

# Pattern rules for object files
%.o: %.cpp
	@echo $(notdir $<)
	@$(CXX) -MMD -MP -MF $(DEPSDIR)/$*.d $(CFLAGS) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	@echo $(notdir $<)
	@$(CC) -MMD -MP -MF $(DEPSDIR)/$*.d $(CFLAGS) -c $< -o $@

%.o: %.s
	@echo $(notdir $<)
	@$(AS) $(ASFLAGS) $< -o $@

-include $(DEPSDIR)/*.d
