/********************************************************************************
 *
 * Filename: 
 *
 * Summary:
 *  Memory setup
 * 
 * Modification History:
 *  Date	  By	   Summary
 *  -------- -------- ----------------------------------------------------------
 *  2009-11-2       Figo.zhang	 Original Creation
 *
 ********************************************************************************/



#include <linux/init.h>
#include <linux/mm.h>
#include <linux/bootmem.h>
#include <linux/pfn.h>
#include <linux/sched.h>
#include <linux/ioport.h>
#include <linux/pci.h>
#include <linux/screen_info.h>
#include <linux/string.h>

#include <asm/cpu.h>
#include <asm/bootinfo.h>
#include <asm/page.h>
#include <asm/sections.h>
#include <asm/irq.h>
#include <asm/dma.h>
#include <asm/time.h>
#include <asm/traps.h>
#include <asm/reboot.h>
#include <asm/mips-boards/prom.h>
#include <asm/mach-optrann/optrann_reg.h>
#include <asm/mach-optrann/optrann_irq.h>
#include <asm/mach-optrann/optrann.h>

#ifdef CONFIG_VT
#include <linux/console.h>
#endif

//int prom_argc;
//char **prom_argv, **prom_envp;

int prom_argc;
unsigned long *_prom_argv;
unsigned long *_prom_envp;

#define prom_envp(index) ((char *)(long)_prom_envp[(index)])



extern void mips_reboot_setup(void);
extern void optrann_time_init(void);
extern unsigned long mira_rtc_get_time(void);

const char *get_system_type(void)
{
	return "Opulan optrann";
}

static void optrann_restart(char *command)
{
	unsigned int reg;
	volatile unsigned int *softres_reg;

	/*disable int*/
	softres_reg = (unsigned int *)ioremap (DISINT_REG, sizeof(unsigned int));
	*softres_reg = 0;
	/*reset*/
	softres_reg = (unsigned int *)ioremap (SOFTRES_REG, sizeof(unsigned int));
	*softres_reg = GORESET;
}

static void optrann_halt(void)
{
	volatile unsigned int *softres_reg = (unsigned int *)ioremap (SOFTRES_REG, sizeof(unsigned int));
	*softres_reg = GORESET;
}



void __init plat_mem_setup(void)
{
	board_time_init = optrann_time_init;
	//rtc_mips_get_time = mira_rtc_get_time;
	_machine_restart = optrann_restart;
	_machine_halt = optrann_halt;
}


#if 0
static int __init get_memsize(char *str)
{
	printk("get memsize str =%s\n", str);
	//get_option(&str, &memsize);
	memsize = simple_strtol(str, NULL, 0);
	printk("get memsize =0x%x\n", memsize);
	return 1;
}
__setup("mem=", get_memsize);
#endif

void __init prom_init(void)
{
	unsigned char *memsize_str;
	unsigned long memsize;

	prom_printf("%s,%s\n", __FILE__,__FUNCTION__);

	char **argv;
	prom_argc = fw_arg0;
	_prom_argv = (unsigned long *) (fw_arg1 | 0x80000000);
	_prom_envp = (unsigned long *) (fw_arg2 | 0x80000000);


	prom_init_cmdline();

	//if ((strstr(prom_getcmdline(), "console=ttyS")) == NULL)
	//	strcat(prom_getcmdline(), "console=ttyS0,9600n8r");

	/*The default memory size is 32M*/
#if 1
	memsize_str = prom_getenv("mem");
	if (!memsize_str)
		memsize = 0x02000000;
	else{
		memsize = simple_strtol(memsize_str, NULL, 0);
		memsize *= 0x100000;
	}
#endif

	printk("RAM memsize = 0x%x\n", memsize);

	/*if memsize > 300MB*/
	if(memsize > 0x12c00000){  
		/*0~ 300MB*/
		add_memory_region(0, 0x12c00000, BOOT_MEM_RAM);
		/*512M ~ memsize*/
		add_memory_region(0x20000000, (memsize - 0x12c00000), BOOT_MEM_RAM);
		
	} else{
		add_memory_region(0, memsize, BOOT_MEM_RAM);
	}
}

void __init prom_free_prom_memory(void)
{
	return 0;
}



