# === Compiler & Flags ===
CC := gcc
INCLUDE := -I include
WARN := -Wall -Wextra -pedantic-errors
STD := -std=c99
DEPFLAGS := -MMD -MP
LDLIBS := -lm
RELEASE_FLAGS := -DNDEBUG -O3

# rpath=$ORIGIN (mark DF_ORIGIN to ensure expansion)
ORIGIN_RPATH := -Wl,-rpath,'$$ORIGIN' -Wl,-z,origin

# === Dirs ===
SRC_DIR := src
BUILD_DIR := deps/release
OBJ_DIR := $(BUILD_DIR)/obj

# === Mains to build together ===
MAINS ?= test/test_wd.c src/wd_process.c
EXE_NAMES := $(basename $(notdir $(MAINS)))
EXES := $(addsuffix .out,$(EXE_NAMES))

# === Sources for lib (exclude any main that lives in src) ===
SRC_CS_ALL := $(wildcard $(SRC_DIR)/*.c)
MAIN_FILES := $(notdir $(MAINS))
SRC_EXCLUDE := $(addprefix $(SRC_DIR)/,$(MAIN_FILES))
SRC_CS := $(filter-out $(SRC_EXCLUDE),$(SRC_CS_ALL))

# === Objects ===
REL_LIB_OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_CS))

# === Outputs (current dir) ===
REL_SO := ./libwd.so

# === Default: build both executables (release) ===
.PHONY: all release clean
.DEFAULT_GOAL := release

all: release
release: $(EXES)

# === Shared lib (PIC) ===
$(REL_SO): $(REL_LIB_OBJS)
	$(CC) -shared -Wl,-soname,libwd.so -o $@ $^ $(LDLIBS)

# Library objects (need -fPIC)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(RELEASE_FLAGS) $(STD) $(WARN) $(INCLUDE) -fPIC $(DEPFLAGS) -c $< -o $@

# === Per-main rules (object + link) ===
define GEN_RULES
$(OBJ_DIR)/$(1).main.o: $(2)
	@mkdir -p $(OBJ_DIR)
	$$(CC) $$(RELEASE_FLAGS) $$(STD) $$(WARN) $$(INCLUDE) $$(DEPFLAGS) -c $$< -o $$@

./$(1).out: $(OBJ_DIR)/$(1).main.o $$(REL_SO)
	$$(CC) $$(RELEASE_FLAGS) $$(STD) $$(WARN) $$(INCLUDE) $$(ORIGIN_RPATH) -L . -o $$@ $(OBJ_DIR)/$(1).main.o -lwd $$(LDLIBS)
endef

$(foreach m,$(MAINS),$(eval $(call GEN_RULES,$(basename $(notdir $(m))),$(m))))

# === Cleanup ===
clean:
	rm -f $(EXES) $(REL_SO)
	rm -rf $(BUILD_DIR)

# === Auto-deps ===
MAIN_DEPS := $(addprefix $(OBJ_DIR)/,$(addsuffix .main.d,$(EXE_NAMES)))
-include $(REL_LIB_OBJS:.o=.d) $(MAIN_DEPS)
