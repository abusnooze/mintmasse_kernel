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
//#include "ad193x.h"
#include "that5173.h"

/* codec private data */
struct that5173_priv {
	struct regmap *regmap;
	//int sysclk;
};


static const struct snd_kcontrol_new that5173_snd_controls[] = {
	

	SOC_DOUBLE_R("Capture Volume", 0x06, 0x07, 0, 20, 0), //maximum value is 20 (= 60 dB gain)

	/*
	#define SOC_DOUBLE_R(xname, reg_left, reg_right, xshift, xmax, xinvert) \
	 {      .iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = (xname), \
	        .info = snd_soc_info_volsw, \
	        .get = snd_soc_get_volsw, .put = snd_soc_put_volsw, \
	        .private_value = SOC_DOUBLE_R_VALUE(reg_left, reg_right, xshift, \
	                                            xmax, xinvert) }
	*/

	/*	
	SOC_DOUBLE_R("DAC1 Volume", AD193X_DAC_L1_VOL,
			AD193X_DAC_R1_VOL, 0, 0xFF, 1),
	SOC_DOUBLE_R("DAC2 Volume", AD193X_DAC_L2_VOL,
			AD193X_DAC_R2_VOL, 0, 0xFF, 1),
	SOC_SINGLE_TLV("Left Playback Volume", LM4857_LVOL, 0, 31, 0,
		stereo_tlv),
	SOC_SINGLE_TLV("Right Playback Volume", LM4857_RVOL, 0, 31, 0,
		stereo_tlv),
	SOC_SINGLE_TLV("Mono Playback Volume", LM4857_MVOL, 0, 31, 0,
		mono_tlv),
	*/
	
};

static int that5173_probe(struct snd_soc_codec *codec)
{
	struct that5173_priv *that5173 = snd_soc_codec_get_drvdata(codec);
	struct snd_soc_dapm_context *dapm = &codec->dapm;
	int ret;
	int tmp; //CS

	printk(KERN_DEBUG "Entering that5173_probe"); //CS

	codec->control_data = that5173->regmap;
	ret = snd_soc_codec_set_cache_io(codec, 0, 0, SND_SOC_REGMAP);
	if (ret < 0) {
		printk(KERN_DEBUG "Failed to set cache I/O: %d\n", ret); //CS
		dev_err(codec->dev, "failed to set cache I/O: %d\n", ret);
		return ret;
	}

	printk(KERN_DEBUG "writing default values to codec registers"); //CS
	
	/*Readback register for debugging-------------*/
	//tmp = snd_soc_read(codec, AD193X_PLL_CLK_CTRL0);
	/*---------------------------------------------*/

	/*Writing default values to registers---------*/
	//snd_soc_write(codec, AD193X_PLL_CLK_CTRL0, 0xd8);
		/*   PLL power-down: normal operation [0 (LSB)]
		     MCKLI/XI pin functionality (PLL active): INPUT 256 (x44.1 or 48 kHz) [00]
		   ? MCKLO/XO pin: off (?) [11]
		     PLL input: ALRCLK [10]
		     Internal MCLK enable: Enable->ADC active [1] 
		     ==> 0xd8*/
	/*--------------------------------------------*/	
	/*Readback register for debugging-------------*/
	//tmp = snd_soc_read(codec, AD193X_PLL_CLK_CTRL0);
	/*---------------------------------------------*/
	
	// how should I handle the daisy chain volume thing. Is it possible with controls?
	snd_soc_add_controls(codec, that5173_snd_controls, ARRAY_SIZE(that5173_snd_controls));
	/*	
	snd_soc_dapm_new_controls(dapm, ad193x_dapm_widgets,
				  ARRAY_SIZE(ad193x_dapm_widgets));
	snd_soc_dapm_add_routes(dapm, audio_paths, ARRAY_SIZE(audio_paths)); 
	*/
	

	return ret;
}


static struct snd_soc_codec_driver soc_codec_dev_that5173 = {
	.probe = 	that5173_probe,
	/*should I define 
	.write = lm4857_write,
	.read = lm4857_read,
	.probe = lm4857_probe,
	.reg_cache_size = ARRAY_SIZE(lm4857_default_regs),
	.reg_word_size = sizeof(uint8_t),
	.reg_cache_default = lm4857_default_regs,
	.set_bias_level = lm4857_set_bias_level,
	?*/
};

static const struct regmap_config that5173_spi_regmap_config = {
	.val_bits = 8,           //Number of bits in a register value, mandatory.
	.reg_bits = 16,          //Number of bits in a register address, mandatory.
	.read_flag_mask = 0x09,  //Mask to be set in the top byte of the register when doing a read.
	.write_flag_mask = 0x08, //Mask to be set in the top byte of the register when doing a write. 
			         //If both read_flag_mask and write_flag_mask are empty the regmap_bus default masks are used.
};

static int __devinit that5173_spi_probe(struct spi_device *spi)
{
	struct that5173_priv *that5173;
	int ret;

	printk(KERN_DEBUG "Entering: that5173_spi_probe"); //CS

	that5173 = kzalloc(sizeof(struct that5173_priv), GFP_KERNEL);
	if (that5173 == NULL)
		return -ENOMEM;

	that5173->regmap = regmap_init_spi(spi, &that5173_spi_regmap_config);
	if (IS_ERR(that5173->regmap)) {
		ret = PTR_ERR(that5173->regmap);
		goto err_free;
	}

	spi_set_drvdata(spi, that5173);

	printk(KERN_DEBUG "that5173.c->that5173_spi_probe: calling snd_soc_register_codec"); //CS
	//ret = snd_soc_register_codec(&spi->dev, &soc_codec_dev_ad193x, &ad193x_dai, 1);
	ret = snd_soc_register_codec(&spi->dev, &soc_codec_dev_that5173, NULL, 0); //NULL, 0 as in lm4857.c
	if (ret < 0)
		goto err_regmap_exit;

	printk(KERN_DEBUG "that5173.c->that5173_spi_probe: successful!"); //CS
	return 0;

err_regmap_exit:
	regmap_exit(that5173->regmap);
err_free:
	kfree(that5173);

	printk(KERN_DEBUG "that5173.c->that5173_spi_probe: return with error"); //CS
	return ret;
}

static int __devexit that5173_spi_remove(struct spi_device *spi)
{
	struct that5173_priv *that5173 = spi_get_drvdata(spi);

	printk(KERN_DEBUG "Entering: that5173_spi_remove"); //CS

	snd_soc_unregister_codec(&spi->dev);
	regmap_exit(that5173->regmap);
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
//#endif


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

