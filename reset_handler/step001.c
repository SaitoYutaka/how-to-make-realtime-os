#include <stdint.h> // uint32_t

/**
 * @brief   Use this macro to define the parts of the vector table
 *
 * The entries in the vector table are sorted in ascending order defined by the
 * (numeric) value given for `x`. The Cortex-M base vectors are always defined
 * with `ISR_VECTOR(0)`, so the CPU specific vector(s) **must** start from 1.
 */
#define ISR_VECTOR(x)   __attribute__((used,section(".vectors." # x )))

extern uint32_t _sfixed;
extern uint32_t _efixed;
extern uint32_t _etext;
extern uint32_t _srelocate;
extern uint32_t _erelocate;
extern uint32_t _szero;
extern uint32_t _ezero;
extern uint32_t _sstack;
extern uint32_t _estack;
extern uint8_t _sram;
extern uint8_t _eram;

/**
 * @brief   Stack size used for the exception (ISR) stack
 * @{
 */
#ifndef ISR_STACKSIZE
#define ISR_STACKSIZE                   (512U)
#endif
/** @} */

/**
 * @brief   Allocation of the interrupt stack
 */
__attribute__((used,section(".isr_stack"))) uint8_t isr_stack[ISR_STACKSIZE];

/**
 * @brief   Number of Cortex-M non-ISR exceptions
 *
 * This means those that are no hardware interrupts, or the ones with a
 * negative interrupt number.
 */
#define CPU_NONISR_EXCEPTIONS   (15)

/**
 * @brief   All ISR functions have this type
 */
typedef void (*isr_t)(void);

/**
 * @brief   Structure of Cortex-M basic vector table
 */
typedef struct {
    void* _estack;                          /**< exception stack pointer */
    isr_t vectors[CPU_NONISR_EXCEPTIONS];   /**< shared Cortex-M vectors */
} cortexm_base_t;


void reset_handler_default(void)
{
    uint32_t *dst;
    const uint32_t *src = &_etext;

    /* load data section from flash to ram */
    for (dst = &_srelocate; dst < &_erelocate; ) {
        *(dst++) = *(src++);
    }

    /* default bss section to zero */
    for (dst = &_szero; dst < &_ezero; ) {
        *(dst++) = 0;
    }
    while(1);
    return;
}

ISR_VECTOR(0) const cortexm_base_t cortex_vector_base = {
    &_estack,
    {
        /* entry point of the program */
        [ 0] = reset_handler_default,
        /* [-14] non maskable interrupt handler */
        // [ 1] = nmi_default,
        // /* [-13] hard fault exception */
        // [ 2] = hard_fault_default,
        // /* [-5] SW interrupt, in RIOT used for triggering context switches */
        // [10] = isr_svc,
        // /* [-2] pendSV interrupt, in RIOT use to do the actual context switch */
        // [13] = isr_pendsv,
        // /* [-1] SysTick interrupt, not used in RIOT */
        // [14] = isr_systick,
    }
};
