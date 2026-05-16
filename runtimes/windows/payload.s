.section .rdata,"dr"
.global _payload_start
.global _payload_end

_payload_start:
    .incbin "payload.bin"
_payload_end: