/*
 * THAT5173 gain control driver supporting THAT5173
 *
 * Copyright 2012 IEM
 *
 * Licensed under the GPL-2 or later.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/i2c.h>
#include <linux/spi/spi.h>
#include <linux/slab.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/initval.h>
#include <sound/soc.h>
#include <sound/tlv.h>
#include "that5173.h"

/* codec private data */
struct that5173_priv {
	struct spi_device *spi;
	struct snd_soc_codec codec;
};

static const uint8_t that5173_default_regs[] = {
	0x00, 0x00, 0x00, 0x00,
};


/* ---------------------------------------------------------------------
 * Register access routines
 */
static unsigned int that5173_reg_read(struct snd_soc_codec *codec,
				   unsigned int reg)
{
	//struct that5173_priv *that5173 = snd_soc_codec_get_drvdata(codec);
	//u8 buffer[8];
	//int rc;

	u16 *cache = codec->reg_cache;
	u16 value;

	printk(KERN_DEBUG "that5173.c->that5173_reg_read: trying to read"); //CS

	value = cache[reg];

	printk(KERN_DEBUG "that5173.c->that5173_reg_read: cached value = %d",value); //CS

	/* this doesn't work: I need to write and read simultaneously. If I use
	   spi_write_then_read(...) I write 64 bit (cached value) and then I read 64 beats
	   (while writing zeros). That is, the registers are in fact zero all the time although
	   I read values different from zero. Hence, for now I just use the cached values and do
	   not perfrom SPI read at all. This is a bit unsafe as I never actually check the register
	   content (I would need to use the scope to really see whats in the THAT5173 register.

	buffer[0] = value; //gain amp 1
	buffer[1] = 0x30;  //-> update on zerocrossing
	buffer[2] = value; //gain amp 2
	buffer[3] = 0x30;  
	buffer[4] = value; //gain amp 3
	buffer[5] = 0x30;
	buffer[6] = value; //gain amp 4
	buffer[7] = 0x30;
	rc = spi_write_then_read(that5173->spi, buffer, 8, buffer, 8);
	if (rc) {
		printk(KERN_DEBUG "that5173.c->that5173_reg_read: SPI read/write error!"); //CS
		return -EIO;
	}
	value = buffer[0];

	printk(KERN_DEBUG "that5173.c->that5173_reg_read: read value = %d",value); //CS
	*/

	/*update cache*/
	cache[reg] = value;

	return value;
}


static unsigned int that5173_reg_read_cache(struct snd_soc_codec *codec,
					 unsigned int reg)
{
	u16 *cache = codec->reg_cache;

	return cache[reg];
}

static int that5173_reg_write(struct snd_soc_codec *codec, unsigned int reg,
			   unsigned int value)
{

	struct that5173_priv *that5173 = snd_soc_codec_get_drvdata(codec);
	u16 *cache = codec->reg_cache;
	u8 buffer[8];
	int rc;

	printk(KERN_DEBUG "that5173.c->that5173_reg_write: trying to write"); //CS


	/*do SPI transfer*/
	buffer[0] = value; //gain amp 1
	buffer[1] = 0x30;  //-> update on zerocrossing
	buffer[2] = value; //gain amp 2
	buffer[3] = 0x30;  
	buffer[4] = value; //gain amp 3
	buffer[5] = 0x30;
	buffer[6] = value; //gain amp 4
	buffer[7] = 0x30;
	rc = spi_write(that5173->spi, buffer, 8);
	if (rc) {
		printk(KERN_DEBUG "that5173.c->that5173_reg_write: SPI reg write error!"); //CS
		return -EIO;
	}
	printk(KERN_DEBUG "that5173.c->that5173_reg_write: success"); //CS

	/*update chache*/
	cache[reg] = value;
	
	return 0;
}


static const struct snd_kcontrol_new that5173_snd_controls[] = {
	
	SOC_SINGLE("Capture Volume", 0x00, 0, 20, 0), //maximum value is 20 (= 60 dB gain), register address doesn't matter

};

static int that5173_probe(struct snd_soc_codec *codec)
{
	//struct that5173_priv *that5173 = snd_soc_codec_get_drvdata(codec);
	//struct snd_soc_dapm_context *dapm = &codec->dapm;

	printk(KERN_DEBUG "Entering that5173_probe"); //CS

	snd_soc_add_controls(codec, that5173_snd_controls, ARRAY_SIZE(that5173_snd_controls));

	return 0;
}


static struct snd_soc_codec_driver soc_codec_dev_that5173 = {
	.probe = 	that5173_probe,
	.read  =	that5173_reg_read,
	.write = 	that5173_reg_write,
	.reg_cache_size = ARRAY_SIZE(that5173_default_regs),
	.reg_word_size = sizeof(uint8_t),
	.reg_cache_default = that5173_default_regs,
};


static int __devinit that5173_spi_probe(struct spi_device *spi)
{
	struct that5173_priv *that5173;
	int ret;

	printk(KERN_DEBUG "Entering: that5173_spi_probe"); //CS

	//CS: compare: tlv320aic26.c

	/*Allocate driver data */
	that5173 = kzalloc(sizeof(struct that5173_priv), GFP_KERNEL);
	if (that5173 == NULL)
		return -ENOMEM;

	that5173->spi = spi;
	dev_set_drvdata(&spi->dev, that5173);
	//that5173->master = 1; //? don't need that, do I?

	ret = snd_soc_register_codec(&spi->dev, &soc_codec_dev_that5173, NULL, 0);
	if (ret < 0){
		kfree(that5173);
		printk(KERN_DEBUG "that5173.c->that5173_spi_probe: snd_soc_register_codec failed!\n");
	} else {
		ret = 0;
		printk(KERN_DEBUG "that5173.c->that5173_spi_probe: SPI device initialized\n");
	}
	
	return ret;
		
}

static int __devexit that5173_spi_remove(struct spi_device *spi)
{
	struct that5173_priv *that5173 = spi_get_drvdata(spi);

	printk(KERN_DEBUG "Entering: that5173_spi_remove"); //CS

	snd_soc_unregister_codec(&spi->dev);

	kfree(that5173);

	return 0;
}

static struct spi_driver that5173_spi_driver = {
	.driver = {
		.name	= "that5173",
		.owner	= THIS_MODULE,
	},
	.probe		= that5173_spi_probe,
	.remove		= __devexit_p(that5173_spi_remove),
};

static int __init that5173_modinit(void)
{
	int ret;

	printk(KERN_DEBUG "init that5173_modinit : SPI driver..."); //CS
	ret = spi_register_driver(&that5173_spi_driver);
	if (ret != 0) {
		printk(KERN_ERR "Failed to register that5173 SPI driver: %d\n",
				ret);
	} else {
		printk(KERN_DEBUG "that5173 SPI driver registered!");
	}
		
	return ret;
}
module_init(that5173_modinit);

static void __exit that5173_modexit(void)
{
	printk(KERN_DEBUG "that5173_modexit: spi_unregister_driver"); //CS
	spi_unregister_driver(&that5173_spi_driver);

}
module_exit(that5173_modexit);

MODULE_DESCRIPTION("ASoC THAT5173 driver");
MODULE_AUTHOR("Christian Schoerkhuber <cschoerkhuber@gmx.at");
MODULE_LICENSE("GPL");

