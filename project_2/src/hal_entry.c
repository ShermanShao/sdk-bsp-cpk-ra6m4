/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2021-10-10     Sherman       first version
 * 2021-11-03     Sherman       Add icu_sample
 */

#include <rtthread.h>
#include "hal_data.h"
#include <rtdevice.h>

#define LED3_PIN    BSP_IO_PORT_01_PIN_06
#define USER_INPUT  "P105"

void mcuboot_quick_setup();
void art_pi_boot_show_logo(void);

void hal_entry(void)
{
    art_pi_boot_show_logo();

    mcuboot_quick_setup();

}

void irq_callback_test(void *args)
{
    rt_kprintf("\n IRQ00 triggered \n");
}

void icu_sample(void)
{
    /* init */
    rt_uint32_t pin = rt_pin_get(USER_INPUT);
    rt_kprintf("\n pin number : 0x%04X \n", pin);
    rt_err_t err = rt_pin_attach_irq(pin, PIN_IRQ_MODE_RISING, irq_callback_test, RT_NULL);
    if(RT_EOK != err)
    {
        rt_kprintf("\n attach irq failed. \n");
    }
    err = rt_pin_irq_enable(pin, PIN_IRQ_ENABLE);
    if(RT_EOK != err)
    {
        rt_kprintf("\n enable irq failed. \n");
    }
}
MSH_CMD_EXPORT(icu_sample, icu sample);

void mcuboot_quick_setup()
{
#ifdef MCUBOOT_USE_MBED_TLS

    /* Initialize mbedtls. */
    mbedtls_platform_context ctx = {0};
    assert(0 == mbedtls_platform_setup(&ctx));
#elif (defined(MCUBOOT_USE_TINYCRYPT) && defined(RM_MCUBOOT_PORT_USE_TINYCRYPT_ACCELERATION))

    /* Initialize TinyCrypt port. */
    assert(FSP_SUCCESS == RM_TINCYRYPT_PORT_Init());
#elif (defined(MCUBOOT_USE_USER_DEFINED_CRYPTO_STACK))

/* Initialize Custom Crypto (Protected Mode) driver. */
    assert(FSP_SUCCESS == R_SCE_Open(&sce_ctrl, &sce_cfg));
#endif

    /* (Optional, not required if --pad is used during signing) To check for updates, call boot_set_pending. */
    bool update = 0;
    if (update)
    {
        boot_set_pending(0);
    }

    /* Verify the boot image and get its location. */
    struct boot_rsp rsp;
    assert(0 == boot_go(&rsp));

    /* Enter the application. */
    RM_MCUBOOT_PORT_BootApp(&rsp);
}

void art_pi_boot_show_logo(void)
{
    rt_kprintf("\n __  __  ____ _   _ _                 _   \n");
    rt_kprintf("|  \\/  |/ ___| | | | |__   ___   ___ | |_ \n");
    rt_kprintf("| |\\/| | |   | | | | '_ \\ / _ \\ / _ \\| __|\n");
    rt_kprintf("| |  | | |___| |_| | |_) | (_) | (_) | |_ \n");
    rt_kprintf("|_|  |_|\\____|\\___/|_.__/ \\___/ \\___/ \\__|\n");
    rt_kprintf("\n Powered by RT-Thread & mcu-boots.\n\n");
}