#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
};

static const struct modversion_info ____versions[]
__attribute_used__
__attribute__((section("__versions"))) = {
	{ 0x4ff5453b, "struct_module" },
	{ 0xe0bdf36a, "register_chrdev" },
	{ 0xc192d491, "unregister_chrdev" },
	{ 0xc1efde6, "remap_pfn_range" },
	{ 0x8a7d1c31, "high_memory" },
	{ 0xdd132261, "printk" },
	{ 0xcd360d1a, "cpu_data" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "E96D09696184BCAAA9D2C7C");
