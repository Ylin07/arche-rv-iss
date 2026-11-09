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

.PHONY: all clean run test

RISCV_TESTS         ?= test/riscv-tests
RISCV_TEST_CATEGORIES ?= rv64ui rv64um rv64ua
RISCV_TEST_ISA      := $(RISCV_TESTS)/isa
RISCV_TEST_ENV      := $(RISCV_TESTS)/env
RISCV_TEST_BUILD    := $(BUILD_DIR)/riscv-tests
RISCV_CC            ?= riscv64-linux-gnu-gcc
RISCV_OBJCOPY       ?= riscv64-linux-gnu-objcopy
RISCV_CFLAGS        := -march=rv64g -mabi=lp64d -static -mcmodel=medany \
                       -fvisibility=hidden -nostdlib -nostartfiles \
                       -I$(RISCV_TEST_ENV) -I$(RISCV_TEST_ISA)/macros/scalar \
                       -T$(RISCV_TEST_ENV)/link.ld

RISCV_TEST_SOURCES := $(foreach cat,$(RISCV_TEST_CATEGORIES),$(wildcard $(RISCV_TEST_ISA)/$(cat)/*.S))
RISCV_TEST_BINS    := $(foreach cat,$(RISCV_TEST_CATEGORIES),$(foreach src,$(wildcard $(RISCV_TEST_ISA)/$(cat)/*.S),$(RISCV_TEST_BUILD)/$(cat)-p-$(notdir $(patsubst %.S,%,$(src))).bin))

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

# Build one standard riscv-tests binary from its assembly source.
define RISCV_TEST_RULE
$(RISCV_TEST_BUILD)/$(1)-p-%.bin: $(RISCV_TEST_ISA)/$(1)/%.S $(RISCV_TEST_ENV)/link.ld $(RISCV_TEST_ENV)/riscv_test.h $(RISCV_TEST_ENV)/../encoding.h $(RISCV_TEST_ISA)/macros/scalar/test_macros.h
	@mkdir -p $$(dir $$@)
	@echo "[TEST-CC] $$<"
	@$(RISCV_CC) $(RISCV_CFLAGS) $$< -o $$(patsubst %.bin,%.elf,$$@)
	@$(RISCV_OBJCOPY) -O binary $$(patsubst %.bin,%.elf,$$@) $$@
endef
$(foreach cat,$(RISCV_TEST_CATEGORIES),$(eval $(call RISCV_TEST_RULE,$(cat))))

test: all $(RISCV_TEST_BINS)
	@test -n "$(RISCV_TEST_BINS)" || (echo "[test] no riscv-tests found"; exit 1)
	@set -e; for image in $(RISCV_TEST_BINS); do \
		echo "[TEST] $$image"; \
		./$(BUILD_DIR)/$(TARGET) $$image; \
	done
	@echo "[test] riscv-tests PASS"


clean:
	@echo "[CLEAN] $(BUILD_DIR)"
	@rm -rf $(BUILD_DIR)

