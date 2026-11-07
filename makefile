TARGET      ?= rviss
BUILD_DIR   ?= build
SRC_DIR     := src
INC_DIRS    := include

CC          ?= gcc
CFLAGS      := -Wall -Wextra -std=gnu11 $(addprefix -I,$(INC_DIRS))
LDFLAGS     := 
LDLIBS      := 

DEBUG       ?= 1

ifeq ($(DEBUG), 1)
    CFLAGS  += -O0 -g3 -DDEBUG -fsanitize=address,undefined
    LDFLAGS += -fsanitize=address,undefined
else
    CFLAGS  += -O3 -DNDEBUG -flto
    LDFLAGS += -O3 -flto
endif

DEPFLAGS     = -MT $@ -MMD -MP -MF $(BUILD_DIR)/$*.d

SRCS        := $(shell find $(SRC_DIR) -type f -name '*.c')
OBJS        := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))
DEPS        := $(OBJS:.o=.d)

.PHONY: all clean run


all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJS)
	@echo "[LD] $@"
	@$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "[CC] $<"
	@$(CC) $(DEPFLAGS) $(CFLAGS) -c $< -o $@

-include $(DEPS)

run: all
	@./$(BUILD_DIR)/$(TARGET)


clean:
	@echo "[CLEAN] $(BUILD_DIR)"
	@rm -rf $(BUILD_DIR)

