.section .rodata
.align 4
.global _binary_build_soundbank_bin_start
_binary_build_soundbank_bin_start:
.incbin "build/soundbank.bin"
.global _binary_build_soundbank_bin_end
_binary_build_soundbank_bin_end:
