#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

MODULE_DESCRIPTION("Read GDT Kernel Module");
MODULE_LICENSE("GPL");

struct gdt_desc
{
        unsigned short  size;
	unsigned long   address;
} __attribute__((packed));

struct gdt_entry
{
	unsigned short  limit0;
	unsigned short	base0;
	unsigned short	base1: 8, a: 1, rw: 1, dc: 1, e: 1, s: 1, dpl: 2, p: 1;
	unsigned short	limit1: 4, res: 1, l: 1, d: 1, g: 1, base2: 8;
} __attribute__((packed));

struct gdt_system_entry
{
	unsigned short  limit0;
	unsigned short	base0;
	unsigned short	base1: 8, type: 4, s: 1, dpl: 2, p: 1;
	unsigned short	limit1: 4, res: 1, l: 1, d: 1, g: 1, base2: 8;
} __attribute__((packed));

static void print_gdt_entry(struct gdt_entry *entry)
{
        pr_info("\tlimit0: %hu, limit1: %hu\n", entry->limit0, entry->limit1);
        pr_info("\tbase0: %hu, base1: %hu, base2: %hu\n", entry->base0, entry->base1, entry->base2);

        if (entry->s)
                pr_info("\te: %hu, dc: %hu, rw: %hu, a: %hu, s: %hu, dpl: %hu, p: %hu",
                        entry->e, entry->dc, entry->rw, entry->a, entry->s, entry->dpl, entry->p);
        else
                pr_info("\ttype: %hu, s: %hu, dpl: %hu, p: %hu", 
                        ((struct gdt_system_entry *)entry)->type, entry->s, entry->dpl, entry->p);

        pr_info("\tl: %hu, d: %hu, g: %hu\n", entry->l, entry->d, entry->g);
}

static int __init gdt_read_init(void)
{
        int i;

        struct gdt_desc desc;
        struct gdt_entry *entries;

        asm volatile("sgdt %0" : "=m" (desc));

        pr_info("GDT size: %hu", desc.size);
        pr_info("GDT address: 0x%lx", desc.address);

        entries = (struct gdt_entry *)desc.address;

        for (i = 0; i < 16; i++)
        {
                pr_info("Entry number %d\n", i);
                print_gdt_entry(entries + i);
        }

        return 0;
}

static void __exit gdt_read_exit(void)
{
        pr_debug("Bye\n");
}

module_init(gdt_read_init);
module_exit(gdt_read_exit);
