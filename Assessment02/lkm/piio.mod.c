#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

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

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x9c3651cf, "module_layout" },
	{ 0xfe990052, "gpio_free" },
	{ 0x2b9dac7e, "class_unregister" },
	{ 0xda13182a, "device_destroy" },
	{ 0xe957c345, "class_destroy" },
	{ 0x8afd4c71, "device_create" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0x5e220692, "__class_create" },
	{ 0x114878ce, "__register_chrdev" },
	{ 0x5f754e5a, "memset" },
	{ 0x809144ac, "gpiod_get_raw_value" },
	{ 0x93bb0087, "gpiod_set_raw_value" },
	{ 0xd7447c7, "gpiod_direction_output_raw" },
	{ 0xed66b2e0, "gpio_to_desc" },
	{ 0x47229b5c, "gpio_request" },
	{ 0x28cc25db, "arm_copy_from_user" },
	{ 0xf4fa543b, "arm_copy_to_user" },
	{ 0x25d5b051, "try_module_get" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0xe7bae5ff, "module_put" },
	{ 0x7c32d0f0, "printk" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "90E030FB4E2BF9C9077F7CB");
