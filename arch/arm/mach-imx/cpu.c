#include <linux/err.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/slab.h>
#include <linux/sys_soc.h>

#include "hardware.h"
#include "common.h"

unsigned int __mxc_cpu_type;
EXPORT_SYMBOL(__mxc_cpu_type);
unsigned int __mxc_arch_type;
EXPORT_SYMBOL(__mxc_arch_type);

static unsigned int imx_soc_revision;

void mxc_set_cpu_type(unsigned int type)
{
	__mxc_cpu_type = type;
}

void mxc_set_arch_type(unsigned int type)
{
	__mxc_arch_type = type;
}

void imx_set_soc_revision(unsigned int rev)
{
	imx_soc_revision = rev;
}

unsigned int imx_get_soc_revision(void)
{
	return imx_soc_revision;
}

void imx_print_silicon_rev(const char *cpu, int srev)
{
	if (srev == IMX_CHIP_REVISION_UNKNOWN)
		pr_info("CPU identified as %s, unknown revision\n", cpu);
	else
		pr_info("CPU identified as %s, silicon rev %d.%d\n",
				cpu, (srev >> 4) & 0xf, srev & 0xf);
}

void __init imx_set_aips(void __iomem *base)
{
	unsigned int reg;
/*
 * Set all MPROTx to be non-bufferable, trusted for R/W,
 * not forced to user-mode.
 */
	__raw_writel(0x77777777, base + 0x0);
	__raw_writel(0x77777777, base + 0x4);

/*
 * Set all OPACRx to be non-bufferable, to not require
 * supervisor privilege level for access, allow for
 * write access and untrusted master access.
 */
	__raw_writel(0x0, base + 0x40);
	__raw_writel(0x0, base + 0x44);
	__raw_writel(0x0, base + 0x48);
	__raw_writel(0x0, base + 0x4C);
	reg = __raw_readl(base + 0x50) & 0x00FFFFFF;
	__raw_writel(reg, base + 0x50);
}

void __init imx_aips_allow_unprivileged_access(
		const char *compat)
{
	void __iomem *aips_base_addr;
	struct device_node *np;

	for_each_compatible_node(np, NULL, compat) {
		aips_base_addr = of_iomap(np, 0);
		imx_set_aips(aips_base_addr);
	}
}

static unsigned long long __init imx_get_soc_uid(void)
{
	struct device_node *np;
	void __iomem *ocotp_base;
	u64 uid = 0ull;

	if (__mxc_cpu_type == MXC_CPU_IMX6DL || __mxc_cpu_type == MXC_CPU_IMX6SX ||
	    __mxc_cpu_type == MXC_CPU_IMX6Q) {
		np = of_find_compatible_node(NULL, NULL, "fsl,imx6q-ocotp");
	} else if (__mxc_cpu_type == MXC_CPU_IMX6SL) {
		np = of_find_compatible_node(NULL, NULL, "fsl,imx6sl-ocotp");
	} else if (__mxc_cpu_type == MXC_CPU_IMX6UL) {
		np = of_find_compatible_node(NULL, NULL, "fsl,imx6ul-ocotp");
	} else if (__mxc_cpu_type == MXC_CPU_IMX6ULL) {
		np = of_find_compatible_node(NULL, NULL, "fsl,imx6ull-ocotp");;
	} else if (__mxc_cpu_type == MXC_CPU_IMX7D) {
		np = of_find_compatible_node(NULL, NULL, "fsl,imx7d-ocotp");
	} else {
		return uid;
	}

	if (!np) {
		pr_warn("failed to find ocotp node\n");
		return uid;
	}

	ocotp_base = of_iomap(np, 0);
	if (!ocotp_base) {
		pr_warn("failed to map ocotp\n");
		goto put_node;
	}

	uid = readl_relaxed(ocotp_base + 0x420);
	uid = (uid << 0x20);
	uid |= readl_relaxed(ocotp_base + 0x410);

	iounmap(ocotp_base);

put_node:
	of_node_put(np);

	return uid;
}

struct device * __init imx_soc_device_init(void)
{
	struct soc_device_attribute *soc_dev_attr;
	struct soc_device *soc_dev;
	struct device_node *root;
	const char *soc_id;
	int ret;

	soc_dev_attr = kzalloc(sizeof(*soc_dev_attr), GFP_KERNEL);
	if (!soc_dev_attr)
		return NULL;

	soc_dev_attr->family = "Freescale i.MX";

	root = of_find_node_by_path("/");
	ret = of_property_read_string(root, "model", &soc_dev_attr->machine);
	of_node_put(root);
	if (ret)
		goto free_soc;

	switch (__mxc_cpu_type) {
	case MXC_CPU_MX1:
		soc_id = "i.MX1";
		break;
	case MXC_CPU_MX21:
		soc_id = "i.MX21";
		break;
	case MXC_CPU_MX25:
		soc_id = "i.MX25";
		break;
	case MXC_CPU_MX27:
		soc_id = "i.MX27";
		break;
	case MXC_CPU_MX31:
		soc_id = "i.MX31";
		break;
	case MXC_CPU_MX35:
		soc_id = "i.MX35";
		break;
	case MXC_CPU_MX51:
		soc_id = "i.MX51";
		break;
	case MXC_CPU_MX53:
		soc_id = "i.MX53";
		break;
	case MXC_CPU_IMX6SL:
		soc_dev_attr->unique_id = kasprintf(GFP_KERNEL, "%llx", imx_get_soc_uid());
		soc_id = "i.MX6SL";
		break;
	case MXC_CPU_IMX6DL:
		soc_dev_attr->unique_id = kasprintf(GFP_KERNEL, "%llx", imx_get_soc_uid());
		soc_id = "i.MX6DL";
		break;
	case MXC_CPU_IMX6SX:
		soc_dev_attr->unique_id = kasprintf(GFP_KERNEL, "%llx", imx_get_soc_uid());
		soc_id = "i.MX6SX";
		break;
	case MXC_CPU_IMX6Q:
		soc_dev_attr->unique_id = kasprintf(GFP_KERNEL, "%llx", imx_get_soc_uid());

		if (imx_get_soc_revision() == IMX_CHIP_REVISION_2_0)
			soc_id = "i.MX6QP";
		else
			soc_id = "i.MX6Q";
		break;
	case MXC_CPU_IMX6UL:
		soc_dev_attr->unique_id = kasprintf(GFP_KERNEL, "%llx", imx_get_soc_uid());
		soc_id = "i.MX6UL";
		break;
	case MXC_CPU_IMX6ULL:
		soc_dev_attr->unique_id = kasprintf(GFP_KERNEL, "%llx", imx_get_soc_uid());
		soc_id = "i.MX6ULL";
		break;
	case MXC_CPU_IMX7D:
		soc_dev_attr->unique_id = kasprintf(GFP_KERNEL, "%llx", imx_get_soc_uid());
		soc_id = "i.MX7D";
		break;
	case MXC_CPU_IMX6SLL:
		soc_id = "i.MX6SLL";
		break;
	default:
		soc_id = "Unknown";
	}
	soc_dev_attr->soc_id = soc_id;

	soc_dev_attr->revision = kasprintf(GFP_KERNEL, "%d.%d",
					   (imx_soc_revision >> 4) & 0xf,
					   imx_soc_revision & 0xf);
	if (!soc_dev_attr->revision)
		goto free_soc;

	soc_dev = soc_device_register(soc_dev_attr);
	if (IS_ERR(soc_dev))
		goto free_rev;

	return soc_device_to_device(soc_dev);

free_rev:
	kfree(soc_dev_attr->revision);
free_soc:
	kfree(soc_dev_attr);
	return NULL;
}
