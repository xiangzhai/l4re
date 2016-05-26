.section ".init"

la $t9, __pthread_initialize_minimal
jalr $t9

