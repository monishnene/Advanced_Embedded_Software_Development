#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xa88a79f7, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xd23d2495, __VMLINUX_SYMBOL_STR(param_ops_charp) },
	{ 0x5ebaac28, __VMLINUX_SYMBOL_STR(param_ops_uint) },
	{ 0xb5e5f630, __VMLINUX_SYMBOL_STR(del_timer) },
	{ 0xc6c1473f, __VMLINUX_SYMBOL_STR(add_timer) },
	{ 0x1f9b5c7c, __VMLINUX_SYMBOL_STR(init_timer_key) },
	{ 0xefd6cf06, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr0) },
	{ 0x526c3a6c, __VMLINUX_SYMBOL_STR(jiffies) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x87ccca88, __VMLINUX_SYMBOL_STR(mod_timer) },
	{ 0x7f02188f, __VMLINUX_SYMBOL_STR(__msecs_to_jiffies) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "966ADCE213A262C58830736");
