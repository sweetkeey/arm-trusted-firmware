/*
 * Copyright (c) 2013-2014, ARM Limited and Contributors. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of ARM nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <plat_arm_gic.h>
#include "fvp_def.h"
#include "fvp_private.h"


const unsigned int irq_sec_array[] = {
	IRQ_TZ_WDOG,
	IRQ_SEC_PHY_TIMER,
	IRQ_SEC_SGI_0,
	IRQ_SEC_SGI_1,
	IRQ_SEC_SGI_2,
	IRQ_SEC_SGI_3,
	IRQ_SEC_SGI_4,
	IRQ_SEC_SGI_5,
	IRQ_SEC_SGI_6,
	IRQ_SEC_SGI_7
};

const unsigned int num_sec_irqs = sizeof(irq_sec_array) /
		sizeof(irq_sec_array[0]);


/*******************************************************************************
 * Enable secure interrupts and use FIQs to route them. Disable legacy bypass
 * and set the priority mask register to allow all interrupts to trickle in.
 ******************************************************************************/
void fvp_gic_cpuif_setup(unsigned int gicc_base)
{
	plat_common_gic_cpuif_setup(gicc_base, BASE_GICR_BASE);
}

/*******************************************************************************
 * Place the cpu interface in a state where it can never make a cpu exit wfi as
 * as result of an asserted interrupt. This is critical for powering down a cpu
 ******************************************************************************/
void fvp_gic_cpuif_deactivate(unsigned int gicc_base)
{
	plat_common_gic_cpuif_deactivate(gicc_base, BASE_GICR_BASE);
}

/*******************************************************************************
 * Per cpu gic distributor setup which will be done by all cpus after a cold
 * boot/hotplug. This marks out the secure interrupts & enables them.
 ******************************************************************************/
void fvp_gic_pcpu_distif_setup(unsigned int gicd_base)
{
	plat_common_gic_pcpu_distif_setup(gicd_base,
			irq_sec_array,
			num_sec_irqs);
}

/*******************************************************************************
 * Global gic distributor setup which will be done by the primary cpu after a
 * cold boot. It marks out the secure SPIs, PPIs & SGIs and enables them. It
 * then enables the secure GIC distributor interface.
 ******************************************************************************/
void fvp_gic_distif_setup(unsigned int gicd_base)
{
	plat_common_gic_distif_setup(gicd_base,
			irq_sec_array,
			num_sec_irqs);
}

void fvp_gic_setup(void)
{
	unsigned int gicd_base, gicc_base;

	gicd_base = fvp_get_cfgvar(CONFIG_GICD_ADDR);
	gicc_base = fvp_get_cfgvar(CONFIG_GICC_ADDR);

	fvp_gic_cpuif_setup(gicc_base);
	fvp_gic_distif_setup(gicd_base);
}

/*******************************************************************************
 * An ARM processor signals interrupt exceptions through the IRQ and FIQ pins.
 * The interrupt controller knows which pin/line it uses to signal a type of
 * interrupt. The platform knows which interrupt controller type is being used
 * in a particular security state e.g. with an ARM GIC, normal world could use
 * the GICv2 features while the secure world could use GICv3 features and vice
 * versa.
 * This function is exported by the platform to let the interrupt management
 * framework determine for a type of interrupt and security state, which line
 * should be used in the SCR_EL3 to control its routing to EL3. The interrupt
 * line is represented as the bit position of the IRQ or FIQ bit in the SCR_EL3.
 ******************************************************************************/
uint32_t plat_interrupt_type_to_line(uint32_t type, uint32_t security_state)
{
	return plat_common_interrupt_type_to_line(
			fvp_get_cfgvar(CONFIG_GICC_ADDR),
			type, security_state);
}

/*******************************************************************************
 * This function returns the type of the highest priority pending interrupt at
 * the GIC cpu interface.
 ******************************************************************************/
uint32_t plat_ic_get_pending_interrupt_type()
{
	return plat_common_ic_get_pending_interrupt_type(
			fvp_get_cfgvar(CONFIG_GICC_ADDR));
}

/*******************************************************************************
 * This function returns the id of the highest priority pending interrupt at
 * the GIC cpu interface.
 ******************************************************************************/
uint32_t plat_ic_get_pending_interrupt_id()
{
	return plat_common_ic_get_pending_interrupt_id(
			fvp_get_cfgvar(CONFIG_GICC_ADDR));
}

/*******************************************************************************
 * This function reads the GIC cpu interface Interrupt Acknowledge register
 * to start handling the pending interrupt.
 ******************************************************************************/
uint32_t plat_ic_acknowledge_interrupt()
{
	return plat_common_ic_acknowledge_interrupt(
			fvp_get_cfgvar(CONFIG_GICC_ADDR));
}

/*******************************************************************************
 * This function writes the GIC cpu interface End Of Interrupt register with
 * the passed value to finish handling the active interrupt
 ******************************************************************************/
void plat_ic_end_of_interrupt(uint32_t id)
{
	plat_common_ic_end_of_interrupt(
			fvp_get_cfgvar(CONFIG_GICC_ADDR), id);
}

/*******************************************************************************
 * This function returns the type of the interrupt id depending upon the group
 * this interrupt has been configured under by the interrupt controller i.e.
 * group0 or group1.
 ******************************************************************************/
uint32_t plat_ic_get_interrupt_type(uint32_t id)
{
	return plat_common_ic_get_interrupt_type(
			fvp_get_cfgvar(CONFIG_GICD_ADDR), id);
}
