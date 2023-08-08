MAKEFLAGS += --no-print-directory -s

TARGET_NAME = clpl
TARGET = $(OUTDIR)/$(TARGET_NAME)

CC_CPP = g++
CC_C = gcc
CC_ASM = as

MODULE_CACHE = ./gcm.cache

BUILDFLAGS = -MMD

FLAGS = $(BUILDFLAGS) -Wall -Wextra -Wpedantic
CPPFLAGS = $(FLAGS)
CFLAGS = $(FLAGS)
ASMFLAGS = --64

LFLAGS =

RUN_ON_WINDOW_COMMAND = konsole --hide-menubar --hide-tabbar --nofork -e make runWithWait
USE_NEW_WINDOW = false
PRESS_TO_EXIT_MSG = "Program complete, press any key to exit..."

MAINARGS = 

OBJDIR = obj
SRCDIR = src

OUTDIR = bin


CPP_SOURCES := $(shell find . -type f -name '*.cpp')
C_SOURCES := $(shell find . -type f -name '*.c')
ASM_SOURCES := $(shell find . -type f -name '*.s')

OBJECTS += $(subst .cpp,.o, $(subst ./src,./$(OBJDIR),$(CPP_SOURCES)))
OBJECTS += $(subst .c,.o, $(subst ./src,./$(OBJDIR),$(C_SOURCES)))
ASM_OBJECTS := $(subst .s,.o, $(subst ./src,./$(OBJDIR),$(ASM_SOURCES)))

DEPS := $(subst .o,.d, $(OBJECTS))

TARGETS_NO_INCLUDE := clean deepclean cleanmodcache
ifeq ($(filter $(MAKECMDGOALS), $(TARGETS_NO_INCLUDE)),)
	-include $(DEPS)
endif

OUT_FILE = $(TARGET_NAME)_output.txt
OUT = > $(OUT_FILE)

PREFIX = [make]:
DONE = Done\n

.PHONY: default
default: $(TARGET);

$(TARGET): $(OBJECTS) $(ASM_OBJECTS) | $(OUTDIR)
	@printf "$(PREFIX) Linking..."
	@$(CC_CPP) -o $(TARGET) $^ $(LFLAGS)
	@printf " $(DONE)"


$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	@printf "$(PREFIX) Compiling $<..."
	@$(CC_CPP) -c $< -o $@ $(CPPFLAGS) || (printf "\n\nBuild failed at file: $<.\n"; exit 1)
	@printf " $(DONE)"

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	@printf "$(PREFIX) Compiling $<..."
	@$(CC_C) -c $< -o $@ $(CFLAGS) || (printf "\n\nBuild failed at file: $<.\n"; exit 1)
	@printf " $(DONE)"

$(OBJDIR)/%.o: $(SRCDIR)/%.s | $(OBJDIR)
	@printf "$(PREFIX) Assembling $<..."
	@$(CC_ASM) $(ASMFLAGS) $< -o $@ || (printf "\n\nBuild failed at file: $<.\n"; exit 1)
	@printf " $(DONE)"

$(OBJDIR)/%.d: $(OBJDIR)/%.o;

$(OUTDIR):
	@printf "$(PREFIX) Creating output directory..."
	@mkdir -p $@
	@printf " $(DONE)"

$(OBJDIR):
	@printf "$(PREFIX) Creating object directory..."
	@mkdir -p $@
	$(shell rsync -a --include='*/' --exclude='*' $(SRCDIR)/ $(OBJDIR)/)
	@printf " $(DONE)"


.PHONY: clean deepclean cleanmodcache run debug runToFile runWithWait

run: $(TARGET)
	@printf "$(PREFIX) Running target: $(TARGET) $(MAINARGS)\n\n"
	@if ($(USE_NEW_WINDOW)) then $(RUN_ON_WINDOW_COMMAND) 2> /dev/null; else ($(TARGET) $(MAINARGS)); fi;

runToFile: $(TARGET)
	@printf "$(PREFIX) Running target: $(TARGET)\n"
	@printf "$(PREFIX) Redirecting stdout to: $(OUT_FILE)\n\n"
	@$(TARGET) $(MAINARGS) $(OUT)

runWithWait: $(TARGET)
	-@$(TARGET) $(MAINARGS)
	@printf "\n\n\n"
	@read  -r -p $(PRESS_TO_EXIT_MSG) key

clean:
	@printf "$(PREFIX) Deleting output directory..."
	@rm -rf $(OUTDIR)
	@printf " $(DONE)"

deepclean: clean cleanmodcache
	@printf "$(PREFIX) Deleting object directory..."
	@rm -rf $(OBJDIR)
	@printf " $(DONE)"

cleanmodcache:
	@printf "$(PREFIX) Deleting module cache directory..."
	@rm -rf $(MODULE_CACHE)
	@printf " $(DONE)"
