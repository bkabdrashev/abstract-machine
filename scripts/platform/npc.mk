AM_SRCS := riscv/npc/start.S \
           riscv/npc/trm.c \
           riscv/npc/ioe.c \
           riscv/npc/timer.c \
           riscv/npc/input.c \
           riscv/npc/cte.c \
           riscv/npc/trap.S \
           platform/dummy/vme.c \
           platform/dummy/mpe.c

CFLAGS    += -fdata-sections -ffunction-sections
LDSCRIPTS += $(AM_HOME)/scripts/linker.ld
LDFLAGS   += --defsym=_pmem_start=0x80000000 --defsym=_entry_offset=0x0
# LDFLAGS   += --defsym=_pflash_start=0x80000000
LDFLAGS   += --defsym=_ptime_uptime=0x10001000
LDFLAGS   += --gc-sections -e _start

MAINARGS_MAX_LEN = 64
MAINARGS_PLACEHOLDER = the_insert-arg_rule_in_Makefile_will_insert_mainargs_here
CFLAGS += -DMAINARGS_MAX_LEN=$(MAINARGS_MAX_LEN) -DMAINARGS_PLACEHOLDER=$(MAINARGS_PLACEHOLDER)

ELF_BASE := $(IMAGE).base.elf
ELF_RUN  := $(IMAGE).run.elf

$(ELF_BASE): image-dep
	@cp $(IMAGE).elf $(ELF_BASE)

insert-arg: image
	@python $(AM_HOME)/tools/insert-arg.py $(ELF_BASE) $(ELF_RUN) $(MAINARGS_MAX_LEN) $(MAINARGS_PLACEHOLDER) "$(mainargs)"

image: $(ELF_BASE)
	@$(OBJDUMP) -d $(IMAGE).base.elf > $(IMAGE).txt
	@echo + OBJCOPY "->" $(IMAGE_REL).bin
	@$(OBJCOPY) -S --set-section-flags .bss=alloc,contents -O binary $(IMAGE).base.elf $(IMAGE).bin

TESTER_PATH := /home/bekzat/chip_bootcamp
BIN_PATH := /home/bekzat/chip_bootcamp/bin

run: insert-arg
	$(BIN_PATH)/gen.sh $(ELF_RUN)
	$(TESTER_PATH)/build_run.sh fast check bin new.bin "$(cpu)" verbose "$(verbose)" measure "$(measure_path)" delay 1 2
	rm new.bin

.PHONY: insert-arg run
