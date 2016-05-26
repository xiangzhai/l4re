# ARCH:       BUILD_ARCH_mips mips
# ARCHDESCR:  MIPS architecture
# ARCHSELECT: ARCH_ENABLE_STACK_PROTECTOR

config CPU
	string
	default "32r2"


choice
	prompt "Page Size"
	default PAGE_SIZE_16KB

config PAGE_SIZE_16KB
	bool "16KiB Page size"


config PAGE_SIZE_4KB
	bool "4KiB Page size"

endchoice
