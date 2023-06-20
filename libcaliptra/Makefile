# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 Western Digital Corporation or its affiliates.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Set Q= on the command line for verbosity
Q=@

ifeq ($(findstring "debug",$(MAKECMDGOALS)),"debug")
CARGO_OUT = $(abspath ../target/debug)
CARGO_FLAG =
else ifeq ($(findstring "release,$(MAKECMDGOALS)),"release")
CARGO_OUT = $(abspath ../target/release)
CARGO_FLAG = --release
else
CARGO_OUT = $(abspath ../target/debug)
endif

RTL_SOC_IFC_INCLUDE_PATH = ../hw-latest/caliptra-rtl/src/soc_ifc/rtl
IMAGE_BUILDER_PATH = $(abspath ../builder)
CFLAGS += -I$(RTL_SOC_IFC_INCLUDE_PATH) -I$(CARGO_OUT) -I$(abspath inc)

LIBCALIPTRA_CARGO = $(CARGO_OUT)/libcaliptra_hw_model_c_binding.a
LIBCALIPTRA = $(CARGO_OUT)/libcaliptra.a

MODEL_HEADER_CARGO = $(CARGO_OUT)/caliptra_model.h
MODEL_HEADER = $(abspath inc/caliptra_model.h)

SOURCE += src/caliptra_api.c

OBJS := $(patsubst %.c,%.o, $(filter %.c,$(SOURCE)))

TESTS = $(sort $(notdir $(wildcard test/*)))
CLEAN_TESTS = $(addprefix clean-, $(TESTS))

.PHONY:%_test %_run clean $(CLEAN_TESTS)

export CARGO_OUT
export MODEL_HEADER
export CFLAGS
export IMAGE_BUILDER_PATH

debug: all
release: all
tests: $(TESTS)
all: $(LIBCALIPTRA) $(OBJS) $(TESTS)

$(LIBCALIPTRA): $(OBJS) $(LIBCALIPTRA_CARGO)
	@echo [COPY] $(LIBCALIPTRA_CARGO) -\> $(LIBCALIPTRA)
	$(Q)cp $(LIBCALIPTRA_CARGO) $(LIBCALIPTRA)
	@echo [AR] $(LIBCALIPTRA)
	$(Q)$(AR) q $(CARGO_OUT)/libcaliptra.a $(OBJS)

$(LIBCALIPTRA_CARGO) $(MODEL_HEADER_CARGO):
	@echo [CARGO] $(LIBCALIPTRA_CARGO) $(MODEL_HEADER_CARGO)
	$(Q)cd ../hw-model/c-binding
	$(Q)cargo build $(CARGO_FLAG)

$(MODEL_HEADER): $(MODEL_HEADER_CARGO)
	@echo [COPY] $(MODEL_HEADER_CARGO) -\> $(MODEL_HEADER) 
	$(Q)cp $(CARGO_OUT)/caliptra_model.h inc/

$(OBJS): $(SOURCE) $(MODEL_HEADER)
	@echo [CC] $@
	$(Q)$(CC) ${CFLAGS} -c $< -o $@

$(CLEAN_TESTS): FORCE
	@echo [CLEAN] $@
	@$(Q)$(MAKE) Q=$(Q) -C test/$(subst clean-,,$@) -f $(subst clean-,,$@).mk clean

FORCE:

clean: $(CLEAN_TESTS)
	@echo [CLEAN] $(CARGO_OUT) $(MODEL_HEADER) $(OBJS)
	$(Q)$(RM) -r $(CARGO_OUT)
	$(Q)$(RM) $(MODEL_HEADER)
	$(Q)$(RM) $(OBJS)

%_test: $(MODEL_HEADER)
	$(Q)$(MAKE) Q=$(Q) -C test/$*_test -f $*_test.mk

%_run:
	$(Q)$(MAKE) Q=$(Q) -C test/$*_test -f $*_test.mk run
