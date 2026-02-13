# Pucky_LED

I went down a rabbit hole. I am working on a new board, preparing a version of the Meshtastic firmware for it. It's based on nRF52840, and instead of having one or more regular LED, it has a WS2812B. Which is nice, except the bootloader doesn't take that into account – fortunately...

A Neopixel library inside a bootloader would suuuuuuck. But I wanted to show the user whether the board is in DFU or app mode. So indeed turning on the LED would be nice.

So here comes the rabbit hole. Implementing code that talks directly to a WS2812B, specifically as nRF52849. It works by flipping the pin high, then low, for specific, below microsecond, periods. 800, 450 ns and 850, 400 ns. So delay() and digitalWrite are no bueno.

Flipping a pin fast is easy enough.

```c
__disable_irq(); // in Arduino: noInterrupts();
NRF_P0->OUT ^= (1UL << PIN_NUMBER);
__enable_irq(); // in Arduino: interrupts();
```

But waiting the exact-ish sub-microsecond delay is trickier. The best solution is ASM, which offers CPU-cycle grade accuracy. `NOP` of course comes to mind, as an old codger, but apparently on new MCUs, optimization makes it so that they can be skipped. So I went for `AND R0, R0`. It's neutral – it doesn't change any register – and takes 1 cycle, ie at 64 MHz 15.625 ns. So I took each duration required to wait after a pin toggle, divided that by 15.625, and used X times `AND R0, R0`, as required to get as close as possible to the duration. That gave me 2 functions, `one()` and `zero()`.

```c
void zero() {
  // One CPU Cycle Duration: 15.625 nanoseconds
  noInterrupts();
  // High 400 ns
  NRF_P0->OUT ^= (1UL << 25);
  asm volatile(
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 125 ns
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 250 ns
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 375 ns
    "MOV R0, R0; MOV R0, R0;");                                                                         // 406.25 ns
  NRF_P0->OUT ^= (1UL << 25);
  // Low 850 ns
  asm volatile(
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 125 ns
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 250 ns
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 375 ns
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 500 ns
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 625 ns
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 750 ns
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; ");                        // 843.75 ns
  interrupts();
}

void one() {
  // One CPU Cycle Duration: 15.625 nanoseconds
  noInterrupts();
  // High 800 ns
  NRF_P0->OUT ^= (1UL << 25);
  asm volatile(
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 125 ns
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 250 ns
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 375 ns
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 500 ns
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 625 ns
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 750 ns
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; ");                                                // 796.875 ns
  NRF_P0->OUT ^= (1UL << 25);
  // Low 450 ns
  asm volatile(
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 125 ns
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 250 ns
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 375 ns
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; ");                                    // 453.125 ns
  interrupts();
}
```

This is an `RGB` LED, so of course I was sending r, the g, then b. After a little adjustment, as I was getting incorrect colours randoml;y (turns out a few ns too many is better than too few), I realized my mistake, which the docs confirmed. It's GRB...

Then I went tweaking the code to make it as compact as possible. It's a bootloader. Space is limited. The code doesn't add much, and the demo Arduino code works well.

The modified bootloader is, for now, under NDA, so, sorry 😐


![Marsup.jpg](Marsup.jpg)

