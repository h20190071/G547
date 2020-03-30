#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/usb.h>	
#include<linux/timer.h>	//handles all usb functionality in kernel
#include<linux/slab.h>

#define KINGSTON_VID 0x0951	//mass storage device 
#define KINGSTON_PID 0x1643

#define MOSERBAER_VID 0x1ec9	//mass storage device of moserbaer pendrive
#define MOSERBAER_PID 0xb101



#define READ_CAPACITY_LENGTH 0x08
#define be_to_int32(buf) (((buf)[0]<<24)|((buf)[1]<<16)|((buf)[2]<<8)|(buf)[3])

#define RETRY_MAX                     5
# define LIBUSB_ERROR_PIPE           -9
#define  SUCCESS		      0
#define log(fmt,...) ({ printk(KERN_INFO "%s: ",__func__); \printk(fmt, ##__VA_ARGS__); \})

enum endpoint_directions {
	USB_ENDP_IN = 0x80,
	USB_ENDP_OUT = 0x00
};


struct usbdev_private		//private structure which is defined for our driver
{
	struct usb_device *udev;  
	unsigned char class;
	unsigned char subclass;
	unsigned char protocol;
	unsigned char ep_in;
	unsigned char ep_out;
};


struct command_block_wrapper {
	uint8_t dCBWSignature[4];
	uint32_t dCBWTag;
	uint32_t dCBWDataTransferLength;
	uint8_t bmCBWFlags;
	uint8_t bCBWLUN;
	uint8_t bCBWCBLength;
	uint8_t CBWCB[16];
};



struct command_status_wrapper {
	uint8_t dCSWSignature[4];
	uint32_t dCSWTag;
	uint32_t dCSWDataResidue;
	uint8_t bCSWStatus;
};


static uint8_t cdb_length[256] = {
//	 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
	06,06,06,06,06,06,06,06,06,06,06,06,06,06,06,06,  //  0
	06,06,06,06,06,06,06,06,06,06,06,06,06,06,06,06,  //  1
	10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  //  2
	10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  //  3
	10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  //  4
	10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  //  5
	00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  6
	00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  7
	16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,  //  8
	16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,  //  9
	12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  //  A
	12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  //  B
	00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  C
	00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  D
	00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  E
	00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  F
};



int send_command_block(struct usb_device*,uint8_t, uint8_t, uint8_t*, uint8_t, uint8_t, uint8_t*);
int get_command_status(struct usb_device*, uint8_t,uint8_t);
int read_capacity(struct usb_device *, uint8_t, uint8_t);


struct usbdev_private *p_usbdev_info;

static void usbdev_disconnect(struct usb_interface *interface)
{
	printk(KERN_INFO "USBDEV Device Removed\n");
	return;
}

static struct usb_device_id usbdev_table [] = {		//device id table
				//passing three devices that driver going to support
	{USB_DEVICE(KINGSTON_VID, KINGSTON_PID)},
	{USB_DEVICE(MOSERBAER_VID, MOSERBAER_PID)},
	{}						//terminating entry
};

int read_capacity(struct usb_device *device, uint8_t ep_in, uint8_t ep_out)
{
	int r, size=0;
	uint8_t lun = 0;
	uint8_t expected_tag;
	long  max_lba, block_size;
	uint8_t cdb[16]={0};	// SCSI Command Descriptor Block
	uint8_t *buffer=NULL;
	long device_size=99;
	
	
	if ( !(buffer = (uint8_t *)kmalloc(sizeof(uint8_t)*64, GFP_KERNEL)) ) {
		printk(KERN_INFO "Cannot allocate memory for rcv buffer\n");
		return -1;
	}
	


	cdb[0] = 0x25;	// Read Capacity
	if(send_command_block(device, ep_out, lun, cdb, USB_ENDP_IN, READ_CAPACITY_LENGTH, &expected_tag) != 0){
		printk(KERN_INFO "Send cdb error\n");
		return -1;
	}
	
	//usb_bulk_msg(device, usb_rcvbulkpipe(device,ep_in), (unsigned char*)buffer, READ_CAPACITY_LENGTH, &size, 1000);



	if(usb_bulk_msg(device, usb_rcvbulkpipe(device,ep_in), (unsigned char*)buffer, READ_CAPACITY_LENGTH, &size, 1000) !=0)
		{
		printk(KERN_INFO "Reading endpoint cdb error\n");
		return -1;
		}
	printk(KERN_INFO "   received %d bytes\n", size);

	max_lba = be_to_int32(buffer);
	block_size = be_to_int32(buffer+4);
	device_size = ((max_lba+1))*block_size/(1024*1024*1024);
	//printk(KERN_INFO "   Max LBA: %08X, Block Size: %08X (%.2f GB)\n", max_lba, block_size, device_size);
	printk(KERN_INFO "Pendrive size in GB -->  %ld GB\n", device_size);
	kfree(buffer);

	if(get_command_status(device, ep_in, expected_tag)==-1){
	printk(KERN_INFO "cannot get command status block\n");
		return -1;
	}
	return 0;



}


int get_command_status(struct usb_device *device, uint8_t endpoint, uint8_t expected_tag)
{
	int i, r, size;
	struct command_status_wrapper *csw;

	i = 0;

	if( !(csw = (struct command_status_wrapper*)kmalloc(sizeof(struct command_status_wrapper), GFP_KERNEL)) ) {
		printk(KERN_INFO "MEMORY ALLOCATION FOR CSW IS FAILED \n");
		return -1;
	}

	do {
		r = usb_bulk_msg(device, usb_rcvbulkpipe(device,endpoint), (unsigned char*)csw, 13, &size, 1000);
		if (r == LIBUSB_ERROR_PIPE) {
			usb_clear_halt(device, usb_sndbulkpipe(device,endpoint));
		}
		i++;
	} while ((r == LIBUSB_ERROR_PIPE) && (i<RETRY_MAX));
		printk(KERN_INFO "COMMAND STATUS WORD RECIEVED %d\n", csw->bCSWStatus);
	kfree(csw);
	
	return r;
}

int send_command_block(struct usb_device *device, uint8_t endpoint, uint8_t lun,uint8_t *cdb, uint8_t direction, uint8_t data_length, uint8_t*ret_tag)
{
	 uint8_t tag = 100;
	 uint8_t cdb_len;
	 int i=0, r, size=0;
	 struct command_block_wrapper *cbw=NULL;

	cbw = (struct command_block_wrapper*)kmalloc(sizeof(struct command_block_wrapper), GFP_KERNEL);
	if ( cbw == NULL ) {
		printk(KERN_INFO "MEMORY ALLOCATION FAILED FOR CBW\n");
		return -1;
	}

	if (cdb == NULL) {
		return -1;
	}

	if (endpoint & USB_ENDP_IN) {
		printk(KERN_INFO "send_mass_storage_command: cannot send command on IN endpoint\n");
		return -1;
	}

	cdb_len = cdb_length[cdb[0]];
	printk(KERN_INFO "cdb_len-> %d\n",cdb_len);
	//if ((cdb_len == 0) || (cdb_len > sizeof(cbw.CBWCB))) {
		//printk(KERN_INFO "send_mass_storage_command: don't know how to handle this command (%02X, length %d)\n",
		//		cdb[0], cdb_len);
		//return -1;
	//}
	
	memset(cbw, 0, sizeof(struct command_block_wrapper));
	cbw->dCBWSignature[0] = 'U';
	cbw->dCBWSignature[1] = 'S';
	cbw->dCBWSignature[2] = 'B';
	cbw->dCBWSignature[3] = 'C';
	*ret_tag = tag;
	cbw->dCBWTag = tag++;
	cbw->dCBWDataTransferLength = data_length;
	cbw->bmCBWFlags = direction;
	cbw->bCBWLUN = lun;
	// Subclass is 1 or 6 => cdb_len
	cbw->bCBWCBLength = cdb_len;
	memcpy(cbw->CBWCB, cdb, cdb_len);
	usb_reset_device(device); 

	i = 0;

	do {
		// The transfer length must always be exactly 31 bytes.
		r = usb_bulk_msg(device, usb_sndbulkpipe(device,endpoint), (unsigned char*)cbw, 31, &size, 1000);
		if (r == LIBUSB_ERROR_PIPE) {
			usb_clear_halt(device, usb_sndbulkpipe(device,endpoint));
		}
		i++;
	} while ((r == LIBUSB_ERROR_PIPE) && (i<RETRY_MAX));
		printk(KERN_INFO "r = %d\n",r);
	if (r != SUCCESS) {
		printk(KERN_INFO "   send_mass_storage_command error\n",r);
		return -1;
	}

	printk(KERN_INFO "   sent successfully %d CDB bytes\n", cdb_len);
	kfree(cbw);
	return 0;
}


static int usbdev_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	int i;
	unsigned char epAddr, epAttr;
	uint8_t ep_in, ep_out; 	
	//struct usb_host_interface *if_desc;
	struct usb_endpoint_descriptor *ep_desc;

	struct usb_device *device;	//standard structure which holds endpoint descriptor of any endpoint________*ep_desc:holds the descriptor


	device = interface_to_usbdev(interface);
	if ( device == NULL ) {
		printk(KERN_INFO "Device is not feteched\n");
		return -1;
	}

//2nd argument : usb_device_id gives the pid and vid that is passed with probe function
	
	ep_in = ep_out = 0;
	
	
	
	if(id->idProduct == KINGSTON_PID && id->idVendor == KINGSTON_VID)
	{
		printk(KERN_INFO "KINGSTON Mass Storage Plugged in\n");
	}


	if(id->idProduct == MOSERBAER_PID && id->idVendor == MOSERBAER_VID)
	{
		printk(KERN_INFO "MOSERBAER Mass Storage Plugged in\n");
	}

	//if_desc = interface->cur_altsetting;
	printk(KERN_INFO "VID:PID: %d:%d\n", id->idVendor, id->idProduct);
	printk(KERN_INFO "No. of Altsettings = %d\n", interface->num_altsetting);
	printk(KERN_INFO "No. of Endpoints = %d\n", interface->cur_altsetting->desc.bNumEndpoints);

	for(i = 0; i < interface->cur_altsetting->desc.bNumEndpoints;i++)
	{
		ep_desc = &interface->cur_altsetting->endpoint[i].desc; //get endpoint descriptor of current endpoint from interface descriptor
		epAddr = ep_desc->bEndpointAddress;	//fetching direction	
		epAttr = ep_desc->bmAttributes;		//fetching type of endpoint		
	
		if((epAttr & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_BULK)  //check those two bits of attribute field of endpoint descriptor
		{ 
			if(epAddr & 0x80)

			 {
				if(!ep_in)
					ep_in = ep_desc->bEndpointAddress;
				printk(KERN_INFO "IN ENDPOINT having address %d\n",ep_in);
			 }		//checks for IN and OUT
				
			else {
				if(!ep_out)
					ep_out = ep_desc->bEndpointAddress;
				printk(KERN_INFO "OUT ENDPOINT having address %d\n",ep_out);
			}
				
		}
	}

	printk(KERN_INFO "USB DEVICE CLASS : %x", interface->cur_altsetting->desc.bInterfaceClass);
	printk(KERN_INFO "USB DEVICE SUB CLASS : %x", interface->cur_altsetting->desc.bInterfaceSubClass);
	printk(KERN_INFO "USB DEVICE PROTOCOL : %x", interface->cur_altsetting->desc.bInterfaceProtocol);

	if ( read_capacity(device,ep_in, ep_out) !=0 ) {
		printk(KERN_INFO "read capacity error\n");
		return -1;
	}


	

	return 0;
}






/*operation structure*/
static struct usb_driver usbdev_driver = {
	name: "usbdev",				//name of the device
	probe: usbdev_probe,			//whenever device is plugged in
	disconnect: usbdev_disconnect,		//when we remove a device
	id_table: usbdev_table,			//list of devices served by this driver
};


int device_init(void)
{
	printk(KERN_INFO "Akanksha-USBdev driver registered\n");
	usb_register(&usbdev_driver);
	return 0;
}

void device_exit(void)
{
	usb_deregister(&usbdev_driver);
	printk(KERN_NOTICE "unregistering usb_driver\n");
}
module_init(device_init);
module_exit(device_exit);
MODULE_DESCRIPTION("USB module");
MODULE_AUTHOR("AKANKSHA BAJPAI <akankshabajpai36@gmail.com>");
MODULE_LICENSE("GPL");




