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

RISCV_PREFIX ?= riscv64-linux-gnu-
TEST_GROUPS  ?= rv64ui rv64um rv64ua rv64mi rv64si
TEST_DIR     := test/riscv-tests
TEST_BUILD   := $(BUILD_DIR)/tests
TEST_SRCS    := $(foreach group,$(TEST_GROUPS),$(wildcard $(TEST_DIR)/isa/$(group)/*.S))
TEST_BINS    := $(patsubst $(TEST_DIR)/isa/%.S,$(TEST_BUILD)/%.bin,$(TEST_SRCS))
TEST_FLAGS   := -march=rv64g -mabi=lp64d -static -mcmodel=medany \
                -fvisibility=hidden -nostdlib -nostartfiles \
                -I$(TEST_DIR)/env -I$(TEST_DIR)/isa/macros/scalar \
                -T$(TEST_DIR)/env/link.ld

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

$(TEST_BUILD)/%.bin: $(TEST_DIR)/isa/%.S $(TEST_DIR)/env/link.ld
	@mkdir -p $(dir $@)
	@echo "[TEST-CC] $<"
	@$(RISCV_PREFIX)gcc $(TEST_FLAGS) -MMD -MP -MF $@.d -MT $@ $< -o $(@:.bin=.elf)
	@$(RISCV_PREFIX)objcopy -O binary $(@:.bin=.elf) $@

-include $(wildcard $(TEST_BUILD)/*/*.bin.d)

test: all $(TEST_BINS)
	@test -n "$(TEST_BINS)" || (echo "[test] no tests found"; exit 1)
	@passed=0; failed=0; for image in $(TEST_BINS); do \
		tohost=$$($(RISCV_PREFIX)nm -g --defined-only $${image%.bin}.elf | awk '$$3 == "tohost" { print "0x" $$1 }'); \
		if test -n "$$tohost" && ./$(BUILD_DIR)/$(TARGET) $$image $$tohost; then \
			echo "[PASS] $$image"; passed=$$((passed + 1)); \
		else \
			echo "[FAIL] $$image"; failed=$$((failed + 1)); \
		fi; \
	done; \
	echo "[test] PASS=$$passed FAIL=$$failed"; test $$failed -eq 0


clean:
	@echo "[CLEAN] $(BUILD_DIR)"
	@rm -rf $(BUILD_DIR)

