include config.mk

# path macros
OBJ_PATH := obj
SRC_PATH := src
INCLUDE_PATH := include

INCDIRS := $(addprefix -I,$(shell find $(INCLUDE_PATH) -type d -print))
INCDIRS += $(addprefix -I,$(DEP_PATH)/include)

# src files & obj files
SRC := $(shell find $(SRC_PATH) -type f -name "*.cpp")
OBJ := $(subst $(SRC_PATH),$(OBJ_PATH),$(SRC:%.cpp=%.o))
OBJDIRS:=$(dir $(OBJ))

# clean files list
DISTCLEAN_LIST := $(OBJ)
CLEAN_LIST := $(DISTCLEAN_LIST)

CLEAN_DEPS := $(DEPS)

# default rule
default: all

# non-phony targets
$(DEPS): $@
	cd modules/$@ && make -j$(shell echo $$((`nproc`))) && make install

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c*
	@echo "Compiling $<"
	$(CXX) $(COBJFLAGS) -o $@ $< $(INCDIRS)

# phony rules
.PHONY: makedir
makedir:
	@mkdir -p $(OBJDIRS)

.PHONY: all
all: makedir $(DEPS) $(OBJ)

.PHONY: target
target: makedir $(OBJ)

.PHONY: test
test: makedir $(OBJ)

.PHONY: install
install: all
	mkdir -p $(INSTALL_PATH)/obj $(INSTALL_PATH)/include
	cp -f $(OBJ) $(INSTALL_PATH)/obj
	cp -f $(shell find $(INCLUDE_PATH) -type f -name "*.h") $(INSTALL_PATH)/include

# TODO: remove only what we installed
.PHONY: uninstall
uninstall:
	rm -rf $(INSTALL_PATH)/lib $(INSTALL_PATH)/include

.PHONY: clean
clean:
	@echo CLEAN $(CLEAN_LIST)
	@rm -f $(CLEAN_LIST)

.PHONY: distclean
distclean:
	@echo CLEAN $(DISTCLEAN_LIST)
	@rm -f $(DISTCLEAN_LIST)
	@rm -rf $(OBJ_PATH)