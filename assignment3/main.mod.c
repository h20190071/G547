#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

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
	{ 0x10289b50, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x8c03d20c, __VMLINUX_SYMBOL_STR(destroy_workqueue) },
	{ 0x42160169, __VMLINUX_SYMBOL_STR(flush_workqueue) },
	{ 0x7e2d8aa7, __VMLINUX_SYMBOL_STR(del_gendisk) },
	{ 0xa3c58da3, __VMLINUX_SYMBOL_STR(usb_deregister) },
	{ 0xbfd592bf, __VMLINUX_SYMBOL_STR(usb_register_driver) },
	{ 0xad516db3, __VMLINUX_SYMBOL_STR(add_disk) },
	{ 0xe914e41e, __VMLINUX_SYMBOL_STR(strcpy) },
	{ 0x43a53735, __VMLINUX_SYMBOL_STR(__alloc_workqueue_key) },
	{ 0x97b48979, __VMLINUX_SYMBOL_STR(blk_cleanup_queue) },
	{ 0xb5a459dc, __VMLINUX_SYMBOL_STR(unregister_blkdev) },
	{ 0xb9ba0537, __VMLINUX_SYMBOL_STR(alloc_disk) },
	{ 0x9912aa5c, __VMLINUX_SYMBOL_STR(blk_init_queue) },
	{ 0x71a50dbc, __VMLINUX_SYMBOL_STR(register_blkdev) },
	{ 0xf97456ea, __VMLINUX_SYMBOL_STR(_raw_spin_unlock_irqrestore) },
	{ 0x21fb443e, __VMLINUX_SYMBOL_STR(_raw_spin_lock_irqsave) },
	{ 0x1e047854, __VMLINUX_SYMBOL_STR(warn_slowpath_fmt) },
	{ 0xed93f29e, __VMLINUX_SYMBOL_STR(__kunmap_atomic) },
	{ 0xcffbe6ed, __VMLINUX_SYMBOL_STR(kmap_atomic) },
	{ 0x12da5bb2, __VMLINUX_SYMBOL_STR(__kmalloc) },
	{ 0x4aca6841, __VMLINUX_SYMBOL_STR(usb_reset_device) },
	{ 0xdb7305a1, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0x4a619f83, __VMLINUX_SYMBOL_STR(memcpy) },
	{ 0x3dc94913, __VMLINUX_SYMBOL_STR(usb_clear_halt) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x6f4910a0, __VMLINUX_SYMBOL_STR(usb_bulk_msg) },
	{ 0x6076fc51, __VMLINUX_SYMBOL_STR(__blk_end_request_all) },
	{ 0x2b3dfddf, __VMLINUX_SYMBOL_STR(blk_fetch_request) },
	{ 0xb2d48a2e, __VMLINUX_SYMBOL_STR(queue_work_on) },
	{ 0x8b2686df, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0x7cfc958d, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x50eedeb8, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xb4390f9a, __VMLINUX_SYMBOL_STR(mcount) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "946C59E3F05DECE0B87043A");
