Q=@

TEST_TARGET = smoke_test
TEST_SOURCE = smoke_test.c

.PHONY: run

all: $(TEST_TARGET)

TEST_OBJS = $(patsubst %.c,%.o, $(filter %.c,$(TEST_SOURCE)))

CARGO_OUT = ../../../../target/debug
LIBCALIPTRA = $(CARGO_OUT)/libcaliptra.a
CALIPTRA_API = ../../../../libcaliptra/inc/

$(TEST_OBJS): $(TEST_SOURCE)
#$(TEST_TARGET): $(TEST_OBJS)

$(LIBCALIPTRA):
	$(Q)$(MAKE) -C ../../../../libcaliptra

$(TEST_OBJS): $(TEST_SOURCE) $(MODEL_HEADER)
	@echo [CC] $@
	$(Q)$(CC) $(CFLAGS) -I$(CALIPTRA_API) -c $< -o $@

$(TEST_TARGET): $(TEST_OBJS) $(LIBCALIPTRA)
	@echo [LINK] $@
	$(Q)$(CC) -o $(TEST_TARGET) $(TEST_OBJS) $(CFLAGS) -Wl,-L$(CARGO_OUT) -lcaliptra -lpthread -lstdc++ -ldl -lm

run: $(TEST_TARGET)
	@echo [RUN] $(TARGET)
	$(Q)cargo run --manifest-path=$(IMAGE_BUILDER_PATH)/Cargo.toml --bin image -- --rom $(CARGO_OUT)/caliptra_rom.bin --fw $(CARGO_OUT)/image_bundle.bin
	$(Q)./$(TEST_TARGET) -r $(CARGO_OUT)/caliptra_rom.bin -f $(CARGO_OUT)/image_bundle.bin

clean:
	@echo [CLEAN] $(TEST_TARGET)
	$(Q)$(RM) $(TEST_TARGET)
	@echo [CLEAN] $(TEST_OBJS)
	$(Q)$(RM) -r $(TEST_OBJS)
