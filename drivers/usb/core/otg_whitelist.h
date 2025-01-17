// SPDX-License-Identifier: GPL-2.0+
/*
 * drivers/usb/core/otg_whitelist.h
 *
 * Copyright (C) 2004 Texas Instruments
 */

/*
 * This OTG and Embedded Host Whitelist is "Targeted Peripheral List".
 * It should mostly use of USB_DEVICE() or USB_DEVICE_VER() entries..
 *
 * YOU _SHOULD_ CHANGE THIS LIST TO MATCH YOUR PRODUCT AND ITS TESTING!
 */

static struct usb_device_id whitelist_table[] = {
/* Add FSL i.mx whitelist, the default list is for USB Compliance Test */
#if defined(CONFIG_USB_EHSET_TEST_FIXTURE)	\
	|| defined(CONFIG_USB_EHSET_TEST_FIXTURE_MODULE)
#define TEST_SE0_NAK_PID			0x0101
#define TEST_J_PID				0x0102
#define TEST_K_PID				0x0103
#define TEST_PACKET_PID				0x0104
#define TEST_HS_HOST_PORT_SUSPEND_RESUME	0x0106
#define TEST_SINGLE_STEP_GET_DEV_DESC		0x0107
#define TEST_SINGLE_STEP_SET_FEATURE		0x0108
#define TEST_OTG_TEST_DEVICE_SUPPORT		0x0200
{ USB_DEVICE(0x1a0a, TEST_SE0_NAK_PID) },
{ USB_DEVICE(0x1a0a, TEST_J_PID) },
{ USB_DEVICE(0x1a0a, TEST_K_PID) },
{ USB_DEVICE(0x1a0a, TEST_PACKET_PID) },
{ USB_DEVICE(0x1a0a, TEST_HS_HOST_PORT_SUSPEND_RESUME) },
{ USB_DEVICE(0x1a0a, TEST_SINGLE_STEP_GET_DEV_DESC) },
{ USB_DEVICE(0x1a0a, TEST_SINGLE_STEP_SET_FEATURE) },
{ USB_DEVICE(0x1a0a, TEST_OTG_TEST_DEVICE_SUPPORT) },
#endif

#define USB_INTERFACE_CLASS_INFO(cl) \
	.match_flags = USB_DEVICE_ID_MATCH_INT_CLASS, \
	.bInterfaceClass = (cl)

{USB_INTERFACE_CLASS_INFO(USB_CLASS_HUB) },
#if IS_ENABLED(CONFIG_USB_STORAGE) || IS_ENABLED(CONFIG_USB_STORAGE_MODULE)
{USB_INTERFACE_CLASS_INFO(USB_CLASS_MASS_STORAGE) },
#endif
#if IS_ENABLED(CONFIG_USB_HID) || IS_ENABLED(CONFIG_USB_HID_MODULE)
{USB_INTERFACE_CLASS_INFO(USB_CLASS_HID) },
#endif

/* OPT Tester */
{ USB_DEVICE( 0x1a0a, 0x0101 ), }, /* TEST_SE0_NAK */
{ USB_DEVICE( 0x1a0a, 0x0102 ), }, /* Test_J */
{ USB_DEVICE( 0x1a0a, 0x0103 ), }, /* Test_K */
{ USB_DEVICE( 0x1a0a, 0x0104 ), }, /* Test_PACKET */
{ USB_DEVICE( 0x1a0a, 0x0105 ), }, /* Test_FORCE_ENABLE */
{ USB_DEVICE( 0x1a0a, 0x0106 ), }, /* HS_PORT_SUSPEND_RESUME  */
{ USB_DEVICE( 0x1a0a, 0x0107 ), }, /* SINGLE_STEP_GET_DESCRIPTOR setup */
{ USB_DEVICE( 0x1a0a, 0x0108 ), }, /* SINGLE_STEP_GET_DESCRIPTOR execute */

/* Sony cameras */
{ USB_DEVICE_VER(0x054c,0x0010,0x0410, 0x0500), },

/* Memory Devices */
//{ USB_DEVICE( 0x0781, 0x5150 ), }, /* SanDisk */
//{ USB_DEVICE( 0x05DC, 0x0080 ), }, /* Lexar */
//{ USB_DEVICE( 0x4146, 0x9281 ), }, /* IOMEGA */
//{ USB_DEVICE( 0x067b, 0x2507 ), }, /* Hammer 20GB External HD  */
{ USB_DEVICE( 0x0EA0, 0x2168 ), }, /* Ours Technology Inc. (BUFFALO ClipDrive)*/
//{ USB_DEVICE( 0x0457, 0x0150 ), }, /* Silicon Integrated Systems Corp. */

/* HP Printers */
//{ USB_DEVICE( 0x03F0, 0x1102 ), }, /* HP Photosmart 245 */
//{ USB_DEVICE( 0x03F0, 0x1302 ), }, /* HP Photosmart 370 Series */

/* Speakers */
//{ USB_DEVICE( 0x0499, 0x3002 ), }, /* YAMAHA YST-MS35D USB Speakers */
//{ USB_DEVICE( 0x0672, 0x1041 ), }, /* Labtec USB Headset */

{ }	/* Terminating entry */
};

static bool match_int_class(struct usb_device_id *id, struct usb_device *udev)
{
	struct usb_host_config *c;
	int num_configs, i;

	/* Copy the code from generic.c */
	c = udev->config;
	num_configs = udev->descriptor.bNumConfigurations;
	for (i = 0; i < num_configs; (i++, c++)) {
		struct usb_interface_descriptor	*desc = NULL;

		/* It's possible that a config has no interfaces! */
		if (c->desc.bNumInterfaces > 0)
			desc = &c->intf_cache[0]->altsetting->desc;

		if (desc && (desc->bInterfaceClass == id->bInterfaceClass))
			return true;
	}

	return false;
}

static int is_targeted(struct usb_device *dev)
{
	struct usb_device_id	*id = whitelist_table;

	/* HNP test device is _never_ targeted (see OTG spec 6.6.6) */
	if ((le16_to_cpu(dev->descriptor.idVendor) == 0x1a0a &&
	     le16_to_cpu(dev->descriptor.idProduct) == 0xbadd))
		return 0;

	/* OTG PET device is always targeted (see OTG 2.0 ECN 6.4.2) */
	if ((le16_to_cpu(dev->descriptor.idVendor) == 0x1a0a &&
	     le16_to_cpu(dev->descriptor.idProduct) == 0x0200))
		return 1;

	/* Unknown Device Not Supporting HNP */
	if ((le16_to_cpu(dev->descriptor.idVendor) == 0x1a0a &&
		le16_to_cpu(dev->descriptor.idProduct) == 0x0201)) {
		dev_warn(&dev->dev, "Unsupported Device\n");
		return 0;
	}
	/* Unknown Device Supporting HNP */
	if ((le16_to_cpu(dev->descriptor.idVendor) == 0x1a0a &&
		le16_to_cpu(dev->descriptor.idProduct) == 0x0202)) {
		dev_warn(&dev->dev, "Device no Responding\n");
		return 0;
	}

	/* NOTE: can't use usb_match_id() since interface caches
	 * aren't set up yet. this is cut/paste from that code.
	 */
	for (id = whitelist_table; id->match_flags; id++) {
		if ((id->match_flags & USB_DEVICE_ID_MATCH_VENDOR) &&
		    id->idVendor != le16_to_cpu(dev->descriptor.idVendor))
			continue;

		if ((id->match_flags & USB_DEVICE_ID_MATCH_PRODUCT) &&
		    id->idProduct != le16_to_cpu(dev->descriptor.idProduct))
			continue;

		/* No need to test id->bcdDevice_lo != 0, since 0 is never
		   greater than any unsigned number. */
		if ((id->match_flags & USB_DEVICE_ID_MATCH_DEV_LO) &&
		    (id->bcdDevice_lo > le16_to_cpu(dev->descriptor.bcdDevice)))
			continue;

		if ((id->match_flags & USB_DEVICE_ID_MATCH_DEV_HI) &&
		    (id->bcdDevice_hi < le16_to_cpu(dev->descriptor.bcdDevice)))
			continue;

		if ((id->match_flags & USB_DEVICE_ID_MATCH_DEV_CLASS) &&
		    (id->bDeviceClass != dev->descriptor.bDeviceClass))
			continue;

		if ((id->match_flags & USB_DEVICE_ID_MATCH_DEV_SUBCLASS) &&
		    (id->bDeviceSubClass != dev->descriptor.bDeviceSubClass))
			continue;

		if ((id->match_flags & USB_DEVICE_ID_MATCH_DEV_PROTOCOL) &&
		    (id->bDeviceProtocol != dev->descriptor.bDeviceProtocol))
			continue;

		if ((id->match_flags & USB_DEVICE_ID_MATCH_INT_CLASS) &&
		    (!match_int_class(id, dev)))
			continue;

		return 1;
		/* NOTE: can't use usb_match_id() since interface caches
		 * aren't set up yet. this is cut/paste from that code.
		 */
		for (id = whitelist_table; id->match_flags; id++) {
#ifdef DEBUG
			dev_dbg(&dev->dev,
				"ID: V:%04x P:%04x DC:%04x SC:%04x PR:%04x \n",
				id->idVendor,
				id->idProduct,
				id->bDeviceClass,
				id->bDeviceSubClass,
				id->bDeviceProtocol);
#endif

			if ((id->match_flags & USB_DEVICE_ID_MATCH_VENDOR) &&
			    id->idVendor != le16_to_cpu(dev->descriptor.idVendor))
				continue;

			if ((id->match_flags & USB_DEVICE_ID_MATCH_PRODUCT) &&
			    id->idProduct != le16_to_cpu(dev->descriptor.idProduct))
				continue;

			/* No need to test id->bcdDevice_lo != 0, since 0 is never
			   greater than any unsigned number. */
			if ((id->match_flags & USB_DEVICE_ID_MATCH_DEV_LO) &&
			    (id->bcdDevice_lo > le16_to_cpu(dev->descriptor.bcdDevice)))
				continue;

			if ((id->match_flags & USB_DEVICE_ID_MATCH_DEV_HI) &&
			    (id->bcdDevice_hi < le16_to_cpu(dev->descriptor.bcdDevice)))
				continue;

			if ((id->match_flags & USB_DEVICE_ID_MATCH_DEV_CLASS) &&
			    (id->bDeviceClass != dev->descriptor.bDeviceClass))
				continue;

			if ((id->match_flags & USB_DEVICE_ID_MATCH_DEV_SUBCLASS) &&
			    (id->bDeviceSubClass != dev->descriptor.bDeviceSubClass))
				continue;

			if ((id->match_flags & USB_DEVICE_ID_MATCH_DEV_PROTOCOL) &&
			    (id->bDeviceProtocol != dev->descriptor.bDeviceProtocol))
				continue;

			return 1;
		}
	}

	/* add other match criteria here ... */

	/* OTG MESSAGE: report errors here, customize to match your product */
	dev_err(&dev->dev, "device v%04x p%04x is not supported\n",
		le16_to_cpu(dev->descriptor.idVendor),
		le16_to_cpu(dev->descriptor.idProduct));
	return 0;
}

