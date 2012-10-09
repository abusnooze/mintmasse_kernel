/*
 * AD193X Audio Codec driver supporting AD1936/7/8/9
 *
 * Copyright 2010 Analog Devices Inc.
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
#include "ad193x.h"

/* codec private data */
struct ad193x_priv {
	struct regmap *regmap;
	int sysclk;
};

/*
 * AD193X volume/mute/de-emphasis etc. controls
 */

/* //CS commented out
static const char *ad193x_deemp[] = {"None", "48kHz", "44.1kHz", "32kHz"};

static const struct soc_enum ad193x_deemp_enum =
	SOC_ENUM_SINGLE(AD193X_DAC_CTRL2, 1, 4, ad193x_deemp);
*/

static const struct snd_kcontrol_new ad193x_snd_controls[] = {
	/* DAC volume control */

	/*CS
	SOC_DOUBLE_R("DAC1 Volume", AD193X_DAC_L1_VOL,
			AD193X_DAC_R1_VOL, 0, 0xFF, 1),
	SOC_DOUBLE_R("DAC2 Volume", AD193X_DAC_L2_VOL,
			AD193X_DAC_R2_VOL, 0, 0xFF, 1),
	SOC_DOUBLE_R("DAC3 Volume", AD193X_DAC_L3_VOL,
			AD193X_DAC_R3_VOL, 0, 0xFF, 1),
	SOC_DOUBLE_R("DAC4 Volume", AD193X_DAC_L4_VOL,
			AD193X_DAC_R4_VOL, 0, 0xFF, 1),
	*/

	/* ADC switch control */
	SOC_DOUBLE("ADC1 Switch", AD193X_ADC_CTRL0, AD193X_ADCL1_MUTE,
		AD193X_ADCR1_MUTE, 1, 1),
	SOC_DOUBLE("ADC2 Switch", AD193X_ADC_CTRL0, AD193X_ADCL2_MUTE,
		AD193X_ADCR2_MUTE, 1, 1),

	/* DAC switch control */
	/*CS
	SOC_DOUBLE("DAC1 Switch", AD193X_DAC_CHNL_MUTE, AD193X_DACL1_MUTE,
		AD193X_DACR1_MUTE, 1, 1),
	SOC_DOUBLE("DAC2 Switch", AD193X_DAC_CHNL_MUTE, AD193X_DACL2_MUTE,
		AD193X_DACR2_MUTE, 1, 1),
	SOC_DOUBLE("DAC3 Switch", AD193X_DAC_CHNL_MUTE, AD193X_DACL3_MUTE,
		AD193X_DACR3_MUTE, 1, 1),
	SOC_DOUBLE("DAC4 Switch", AD193X_DAC_CHNL_MUTE, AD193X_DACL4_MUTE,
		AD193X_DACR4_MUTE, 1, 1),
	*/

	/* ADC high-pass filter */
	SOC_SINGLE("ADC High Pass Filter Switch", AD193X_ADC_CTRL0,
			AD193X_ADC_HIGHPASS_FILTER, 1, 0),

	/* DAC de-emphasis */
	//SOC_ENUM("Playback Deemphasis", ad193x_deemp_enum), //CS
};

static const struct snd_soc_dapm_widget ad193x_dapm_widgets[] = {
	//SND_SOC_DAPM_DAC("DAC", "Playback", AD193X_DAC_CTRL0, 0, 1),
	SND_SOC_DAPM_ADC("ADC", "Capture", SND_SOC_NOPM, 0, 0), //SND_SOC_NOPM=-1 ... widget has no PM register bit
	//SND_SOC_DAPM_SUPPLY("PLL_PWR", AD193X_PLL_CLK_CTRL0, 0, 1, NULL, 0), //CS: test: get rid of the supply dapm widget...
	//SND_SOC_DAPM_SUPPLY("ADC_PWR", AD193X_ADC_CTRL0, 0, 1, NULL, 0),	//CS: test: get rid of the supply dapm widget...
	//SND_SOC_DAPM_OUTPUT("DAC1OUT"),
	//SND_SOC_DAPM_OUTPUT("DAC2OUT"),
	//SND_SOC_DAPM_OUTPUT("DAC3OUT"),
	//SND_SOC_DAPM_OUTPUT("DAC4OUT"),
	SND_SOC_DAPM_INPUT("ADC1IN"),
	SND_SOC_DAPM_INPUT("ADC2IN"),
};

static const struct snd_soc_dapm_route audio_paths[] = {
	//{ "DAC", NULL, "PLL_PWR" },
	//{ "ADC", NULL, "PLL_PWR" }, //CS: test: get rid of the supply dapm widget...(nur gemeinsam mit dem zeug oben ein/auskommentieren)
	//{ "DAC", NULL, "ADC_PWR" },
	//{ "ADC", NULL, "ADC_PWR" }, //CS: test: get rid of the supply dapm widget...(nur gemeinsam mit dem zeug oben ein/auskommentieren)
	//{ "DAC1OUT", "DAC1 Switch", "DAC" },
	//{ "DAC2OUT", "DAC2 Switch", "DAC" },
	//{ "DAC3OUT", "DAC3 Switch", "DAC" },
	//{ "DAC4OUT", "DAC4 Switch", "DAC" },
	{ "ADC", "ADC1 Switch", "ADC1IN" },
	{ "ADC", "ADC2 Switch", "ADC2IN" },
};

/*
 * DAI ops entries
 */

static int ad193x_mute(struct snd_soc_dai *dai, int mute)
{
	struct snd_soc_codec *codec = dai->codec;

	
	printk(KERN_DEBUG "Entering ad193x.c->ad193x_mute...do nothing\n"); //CS
	/* //CS: commented out
	if (mute)
		snd_soc_update_bits(codec, AD193X_DAC_CTRL2,
				    AD193X_DAC_MASTER_MUTE,
				    AD193X_DAC_MASTER_MUTE);
	else
		snd_soc_update_bits(codec, AD193X_DAC_CTRL2,
				    AD193X_DAC_MASTER_MUTE, 0);
	*/
	printk(KERN_DEBUG "Exit ad193x.c->ad193x_mute\n"); //CS
	return 0;
}

static int ad193x_set_tdm_slot(struct snd_soc_dai *dai, unsigned int tx_mask,
			       unsigned int rx_mask, int slots, int width)
{
	struct snd_soc_codec *codec = dai->codec;
	int tmp = 0; //CS
	//int dac_reg = snd_soc_read(codec, AD193X_DAC_CTRL1); //CS
	int adc_reg = snd_soc_read(codec, AD193X_ADC_CTRL2);

	printk(KERN_DEBUG "Entering ad193x.c->ad193x_set_tdm_slot\n"); //CS
	printk(KERN_DEBUG "set_tdm_slot: Read codec registers: AD193X_ADC_CTRL2=%#x\n", adc_reg); //CS

	//dac_reg &= ~AD193X_DAC_CHAN_MASK;
	adc_reg &= ~AD193X_ADC_CHAN_MASK;

	switch (slots) {
	case 2:
		//dac_reg |= AD193X_DAC_2_CHANNELS << AD193X_DAC_CHAN_SHFT;
		adc_reg |= AD193X_ADC_2_CHANNELS << AD193X_ADC_CHAN_SHFT;  //setting bit clocks per frame to 64
		tmp = AD193X_ADC_2_CHANNELS << AD193X_ADC_CHAN_SHFT;
		printk(KERN_DEBUG "tdm slots: 2, ADC_CTRL2 |= %#x\n",tmp); //CS
		break;
	case 4:
		//dac_reg |= AD193X_DAC_4_CHANNELS << AD193X_DAC_CHAN_SHFT; 
		adc_reg |= AD193X_ADC_4_CHANNELS << AD193X_ADC_CHAN_SHFT; //setting bit clocks per frame to 128
		printk(KERN_DEBUG "tdm slots: 4, ADC_CTRL2 |= %#x\n",tmp); //CS
		break;
	case 8:
		//dac_reg |= AD193X_DAC_8_CHANNELS << AD193X_DAC_CHAN_SHFT;
		adc_reg |= AD193X_ADC_8_CHANNELS << AD193X_ADC_CHAN_SHFT; //setting bit clocks per frame to 256
		printk(KERN_DEBUG "tdm slots: 8, ADC_CTRL2 |= %#x\n",tmp); //CS
		break;
	case 16:
		//dac_reg |= AD193X_DAC_16_CHANNELS << AD193X_DAC_CHAN_SHFT;
		adc_reg |= AD193X_ADC_16_CHANNELS << AD193X_ADC_CHAN_SHFT; //setting bit clocks per frame to 512
		printk(KERN_DEBUG "tdm slots: 16, ADC_CTRL2 |= %#x\n",tmp); //CS
		break;
	default:
		printk(KERN_DEBUG "returning -EINVAL: not writing to codec!"); //CS
		return -EINVAL;
	}

	printk(KERN_DEBUG "Writing codec registers: AD193X_ADC_CTRL2=%#x\n", adc_reg); //CS
	//snd_soc_write(codec, AD193X_DAC_CTRL1, dac_reg); //CS
	snd_soc_write(codec, AD193X_ADC_CTRL2, adc_reg);

	printk(KERN_DEBUG "Exit ad193x.c->ad193x_set_tdm_slot\n"); //CS
	return 0;
}

static int ad193x_set_dai_fmt(struct snd_soc_dai *codec_dai,
		unsigned int fmt)
{
	struct snd_soc_codec *codec = codec_dai->codec;
	int adc_reg1, adc_reg2;
	int pll_reg1;
	//int dac_reg;
	int regi0, regi1, regi2, regi3, regi4, regi5, regi6, regi7; //only for test
	int tmp = 0;
	printk(KERN_DEBUG "Entering ad193x.c->ad193x_set_dai_fmt"); //CS


	/*Power up PLL and ADC. Shouldn't that be done in
	  snd_pcm_open (userspace) as they are powered down
	  via snd_pcm_close (userspace)?-------------------*/
	printk(KERN_DEBUG "Power up ADC and PLL (shouldn't that be done elsewhere?)");
	regi0 = snd_soc_read(codec, AD193X_PLL_CLK_CTRL0);
	regi5 = snd_soc_read(codec, AD193X_ADC_CTRL0);
	regi0 &= 0xfe; //clear LSB -> power up PLL
	regi5 &= 0xfe; //clear LSB -> power up ADC
	snd_soc_write(codec, AD193X_PLL_CLK_CTRL0, regi0);
	snd_soc_write(codec, AD193X_ADC_CTRL0, regi5);
	/*-------------------------------------------------*/

	/*TEST: read all registers---------------*/
	regi0 = snd_soc_read(codec, AD193X_PLL_CLK_CTRL0);
	regi1 = snd_soc_read(codec, AD193X_PLL_CLK_CTRL1);
	regi2 = snd_soc_read(codec, AD193X_AUXPORT_CTRL0);
	regi3 = snd_soc_read(codec, AD193X_AUXPORT_CTRL1);
	regi4 = snd_soc_read(codec, AD193X_AUXPORT_CTRL2);  
	regi5 = snd_soc_read(codec, AD193X_ADC_CTRL0);
	regi6 = snd_soc_read(codec, AD193X_ADC_CTRL1);
	regi7 = snd_soc_read(codec, AD193X_ADC_CTRL2);
	printk(KERN_DEBUG "readtest: read %#x => %#x\n", AD193X_PLL_CLK_CTRL0, regi0);
	printk(KERN_DEBUG "readtest: read %#x => %#x\n", AD193X_PLL_CLK_CTRL1, regi1);
	printk(KERN_DEBUG "readtest: read %#x => %#x\n", AD193X_AUXPORT_CTRL0, regi2);
	printk(KERN_DEBUG "readtest: read %#x => %#x\n", AD193X_AUXPORT_CTRL1, regi3);
	printk(KERN_DEBUG "readtest: read %#x => %#x\n", AD193X_AUXPORT_CTRL2, regi4);
	printk(KERN_DEBUG "readtest: read %#x => %#x\n", AD193X_ADC_CTRL0, regi5);
	printk(KERN_DEBUG "readtest: read %#x => %#x\n", AD193X_ADC_CTRL1, regi6);
 	printk(KERN_DEBUG "readtest: read %#x => %#x\n", AD193X_ADC_CTRL2, regi7);
	/*--------------------------------------*/

	/*TEST: Write default register settings*/
	/*
	snd_soc_write(codec, AD193X_PLL_CLK_CTRL0, 0);
	snd_soc_write(codec, AD193X_PLL_CLK_CTRL1, 8);
	snd_soc_write(codec, AD193X_AUXPORT_CTRL0, 0);
	snd_soc_write(codec, AD193X_AUXPORT_CTRL1, 0);
	snd_soc_write(codec, AD193X_AUXPORT_CTRL2, 0);  
	snd_soc_write(codec, AD193X_ADC_CTRL0, 0);
	snd_soc_write(codec, AD193X_ADC_CTRL1, 0);
	snd_soc_write(codec, AD193X_ADC_CTRL2, 0);
	*/
	/*--------------------------------------*/	

	/*TEST: read all registers---------------*/
	/*
	regi0 = snd_soc_read(codec, AD193X_PLL_CLK_CTRL0);
	regi1 = snd_soc_read(codec, AD193X_PLL_CLK_CTRL1);
	regi2 = snd_soc_read(codec, AD193X_AUXPORT_CTRL0);
	regi3 = snd_soc_read(codec, AD193X_AUXPORT_CTRL1);
	regi4 = snd_soc_read(codec, AD193X_AUXPORT_CTRL2);  
	regi5 = snd_soc_read(codec, AD193X_ADC_CTRL0);
	regi6 = snd_soc_read(codec, AD193X_ADC_CTRL1);
	regi7 = snd_soc_read(codec, AD193X_ADC_CTRL2);
	printk(KERN_DEBUG "readtest: read %#x => %#x\n", AD193X_PLL_CLK_CTRL0, regi0);
	printk(KERN_DEBUG "readtest: read %#x => %#x\n", AD193X_PLL_CLK_CTRL1, regi1);
	printk(KERN_DEBUG "readtest: read %#x => %#x\n", AD193X_AUXPORT_CTRL0, regi2);
	printk(KERN_DEBUG "readtest: read %#x => %#x\n", AD193X_AUXPORT_CTRL1, regi3);
	printk(KERN_DEBUG "readtest: read %#x => %#x\n", AD193X_AUXPORT_CTRL2, regi4);
	printk(KERN_DEBUG "readtest: read %#x => %#x\n", AD193X_ADC_CTRL0, regi5);
	printk(KERN_DEBUG "readtest: read %#x => %#x\n", AD193X_ADC_CTRL1, regi6);
 	printk(KERN_DEBUG "readtest: read %#x => %#x\n", AD193X_ADC_CTRL2, regi7);
	*/
	/*--------------------------------------*/


	/*TEST---------------*/
	/*
	pll_reg1 = snd_soc_read(codec, AD193X_PLL_CLK_CTRL1); 
	printk(KERN_DEBUG "test: read AD193X_PLL_CLK_CTRL1 => value: %#x", pll_reg1); //CS
	pll_reg1 = 9;
	printk(KERN_DEBUG "test: write AD193X_PLL_CLK_CTRL1 => value: %#x", pll_reg1); //CS
	snd_soc_write(codec, AD193X_PLL_CLK_CTRL1, pll_reg1);
	pll_reg1 = snd_soc_read(codec, AD193X_PLL_CLK_CTRL1);
	printk(KERN_DEBUG "test: read back AD193X_PLL_CLK_CTRL1 => value: %#x", pll_reg1); //CS
	*/
	/*-------------------*/

	
	adc_reg1 = snd_soc_read(codec, AD193X_ADC_CTRL1);
	adc_reg2 = snd_soc_read(codec, AD193X_ADC_CTRL2);
	//dac_reg = snd_soc_read(codec, AD193X_DAC_CTRL1);

	
	printk(KERN_DEBUG "set_dai_fmt: Read codec registers: AD193X_ADC_CTRL1=%#x, AD193X_ADC_CTRL2=%#x\n",adc_reg1,adc_reg2); //CS

	/* At present, the driver only support AUX ADC mode(SND_SOC_DAIFMT_I2S
	 * with TDM) and ADC&DAC TDM mode(SND_SOC_DAIFMT_DSP_A)
	 */

	/* CS: drop that switch for now (just use default values defined above)
	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S: //ADC Control 1 -> x01x xxxx -> TDM (daisy chain)
		printk(KERN_DEBUG "case SND_SOC_DAIFMT_I2S [ad193x: TDM (daisy chain)]"); //CS
		adc_reg1 &= ~AD193X_ADC_SERFMT_MASK;
		adc_reg1 |= AD193X_ADC_SERFMT_TDM;
		
		break;
	case SND_SOC_DAIFMT_DSP_A: //ADC Control 1 -> x10x xxxx -> ADC AUX mode (TDM-coupled)
		printk(KERN_DEBUG "case SND_SOC_DAIFMT_DSP_A [ad193x: ADC AUX mode (TDM-coupled)]"); //CS
		adc_reg1 &= ~AD193X_ADC_SERFMT_MASK;
		adc_reg1 |= AD193X_ADC_SERFMT_AUX;
		break;
	default:
		printk(KERN_DEBUG "switch1: return -EINVAL"); //CS
		return -EINVAL;
	} */

	switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
	case SND_SOC_DAIFMT_NB_NF: /* normal bit clock + frame */
		printk(KERN_DEBUG "normal bit clock + frame\n"); //CS
		adc_reg2 &= ~AD193X_ADC_LEFT_HIGH; //AD193X_ADC_LEFT_HIGH =0000 0100
		adc_reg2 &= ~AD193X_ADC_BCLK_INV;  //AD193X_ADC_BCLK_INV  =0000 0010
		//dac_reg &= ~AD193X_DAC_LEFT_HIGH;
		//dac_reg &= ~AD193X_DAC_BCLK_INV;
		break;
	case SND_SOC_DAIFMT_NB_IF: /* normal bclk + invert frm */
		printk(KERN_DEBUG "normal bit clock + invert frame\n"); //CS
		adc_reg2 |= AD193X_ADC_LEFT_HIGH;  
		adc_reg2 &= ~AD193X_ADC_BCLK_INV;
		//dac_reg |= AD193X_DAC_LEFT_HIGH;
		//dac_reg &= ~AD193X_DAC_BCLK_INV;
		break;
	case SND_SOC_DAIFMT_IB_NF: /* invert bclk + normal frm */
		printk(KERN_DEBUG "invert bit clock + normal frame\n"); //CS
		adc_reg2 &= ~AD193X_ADC_LEFT_HIGH;
		adc_reg2 |= AD193X_ADC_BCLK_INV;
		//dac_reg &= ~AD193X_DAC_LEFT_HIGH;
		//dac_reg |= AD193X_DAC_BCLK_INV;
		break;

	case SND_SOC_DAIFMT_IB_IF: /* invert bclk + frm */
		printk(KERN_DEBUG "invert bit clock + frame\n"); //CS
		adc_reg2 |= AD193X_ADC_LEFT_HIGH;
		adc_reg2 |= AD193X_ADC_BCLK_INV;
		//dac_reg |= AD193X_DAC_LEFT_HIGH;
		//dac_reg |= AD193X_DAC_BCLK_INV;
		break;
	default:
		printk(KERN_DEBUG "switch2: return -EINVAL"); //CS
		return -EINVAL;
	}

	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM: /* codec clk & frm master */
		printk(KERN_DEBUG "codec clk & frm master\n"); //CS
		/*temp auskommentiert		
		adc_reg2 |= AD193X_ADC_LCR_MASTER;
		adc_reg2 |= AD193X_ADC_BCLK_MASTER;
		//dac_reg |= AD193X_DAC_LCR_MASTER;
		//dac_reg |= AD193X_DAC_BCLK_MASTER; */

		//Nur zu testzwecken: von bone aus gesehen ist der codec clk und frm master,
		//da ich jetzt aber die frm-clock extern generiere ist der codec in wirklichkeit
		//frm slave und clk master (clk wird aus frm per pll erzeugt):
		printk(KERN_DEBUG "...aber jetzt grad ist codec frm slave und clk master\n"); //CS
		adc_reg2 &= ~AD193X_ADC_LCR_MASTER;
		adc_reg2 |= AD193X_ADC_BCLK_MASTER;

		break;
	case SND_SOC_DAIFMT_CBS_CFM: /* codec clk slave & frm master */
		printk(KERN_DEBUG "codec clk slave & frm master\n"); //CS
		adc_reg2 |= AD193X_ADC_LCR_MASTER;
		adc_reg2 &= ~AD193X_ADC_BCLK_MASTER;
		//dac_reg |= AD193X_DAC_LCR_MASTER;
		//dac_reg &= ~AD193X_DAC_BCLK_MASTER;
		break;
	case SND_SOC_DAIFMT_CBM_CFS: /* codec clk master & frame slave */
		printk(KERN_DEBUG "codec clk master & frame slave\n"); //CS
		adc_reg2 &= ~AD193X_ADC_LCR_MASTER;
		adc_reg2 |= AD193X_ADC_BCLK_MASTER;
		//dac_reg &= ~AD193X_DAC_LCR_MASTER;
		//dac_reg |= AD193X_DAC_BCLK_MASTER;
		break;
	case SND_SOC_DAIFMT_CBS_CFS: /* codec clk & frm slave */
		printk(KERN_DEBUG "codec clk & frm slave\n"); //CS
		adc_reg2 &= ~AD193X_ADC_LCR_MASTER;
		adc_reg2 &= ~AD193X_ADC_BCLK_MASTER;
		//dac_reg &= ~AD193X_DAC_LCR_MASTER;
		//dac_reg &= ~AD193X_DAC_BCLK_MASTER;
		break;
	default:
		printk(KERN_DEBUG "switch3: return -EINVAL"); //CS
		return -EINVAL;
	}

	printk(KERN_DEBUG "Writing codec registers: AD193X_ADC_CTRL1=%#x, AD193X_ADC_CTRL2=%#x\n",adc_reg1,adc_reg2); //CS
	snd_soc_write(codec, AD193X_ADC_CTRL1, adc_reg1);
	snd_soc_write(codec, AD193X_ADC_CTRL2, adc_reg2);
	//snd_soc_write(codec, AD193X_DAC_CTRL1, dac_reg);

	/*CS: debugging*/
	adc_reg1 = snd_soc_read(codec, AD193X_ADC_CTRL1); //CS: degubbing
	adc_reg2 = snd_soc_read(codec, AD193X_ADC_CTRL2); //CS: degubbing
	//dac_reg = snd_soc_read(codec, AD193X_DAC_CTRL1); //CS: degubbing
	printk(KERN_DEBUG "debug: Read codec registers: AD193X_ADC_CTRL1=%#x, AD193X_ADC_CTRL2=%#x\n",adc_reg1,adc_reg2); //CS

	return 0;
}

static int ad193x_set_dai_sysclk(struct snd_soc_dai *codec_dai,
		int clk_id, unsigned int freq, int dir)
{
	struct snd_soc_codec *codec = codec_dai->codec;
	struct ad193x_priv *ad193x = snd_soc_codec_get_drvdata(codec);

	printk(KERN_DEBUG "Entering ad193x.c->ad193x_set_dai_sysclk"); //CS

	switch (freq) {
	case 12288000:
	case 18432000:
	case 24576000:
	case 36864000:
		ad193x->sysclk = freq;
		printk(KERN_DEBUG "setting sysclk to %d\n",freq); //CS
		return 0;
	}
	printk(KERN_DEBUG "ERROR (ad193x_set_dai_sysclk): unknown sysclk!"); //CS
	return -EINVAL;
}

static int ad193x_hw_params(struct snd_pcm_substream *substream,
		struct snd_pcm_hw_params *params,
		struct snd_soc_dai *dai)
{
	int word_len = 0, master_rate = 0;

	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_codec *codec = rtd->codec;
	struct ad193x_priv *ad193x = snd_soc_codec_get_drvdata(codec);

	printk(KERN_DEBUG "Entering ad193x.c->ad193x_hw_params"); //CS

	/* bit size */
	switch (params_format(params)) {
	case SNDRV_PCM_FORMAT_S16_LE:
		printk(KERN_DEBUG "ad193x.c->ad193x_hw_params: SNDRV_PCM_FORMAT_S16_LE"); //CS
		word_len = 3;
		break;
	case SNDRV_PCM_FORMAT_S20_3LE:
		printk(KERN_DEBUG "ad193x.c->ad193x_hw_params: SNDRV_PCM_FORMAT_S20_3LE"); //CS
		word_len = 1;
		break;
	case SNDRV_PCM_FORMAT_S24_LE:
	case SNDRV_PCM_FORMAT_S32_LE:
		printk(KERN_DEBUG "ad193x.c->ad193x_hw_params: SNDRV_PCM_FORMAT_S24_LE or SNDRV_PCM_FORMAT_S32_LE"); //CS
		word_len = 0;
		break;
	}

	switch (ad193x->sysclk) {
	case 12288000:
		printk(KERN_DEBUG "ad193x.c->ad193x_hw_params: master_rate = AD193X_PLL_INPUT_256"); //CS
		master_rate = AD193X_PLL_INPUT_256;
		break;
	case 18432000:
		printk(KERN_DEBUG "ad193x.c->ad193x_hw_params: master_rate = AD193X_PLL_INPUT_384"); //CS
		master_rate = AD193X_PLL_INPUT_384;
		break;
	case 24576000:
		printk(KERN_DEBUG "ad193x.c->ad193x_hw_params: master_rate = AD193X_PLL_INPUT_512;"); //CS
		master_rate = AD193X_PLL_INPUT_512;
		break;
	case 36864000:
		printk(KERN_DEBUG "ad193x.c->ad193x_hw_params: master_rate = AD193X_PLL_INPUT_768"); //CS
		master_rate = AD193X_PLL_INPUT_768;
		break;
	}

	snd_soc_update_bits(codec, AD193X_PLL_CLK_CTRL0,
			    AD193X_PLL_INPUT_MASK, master_rate); //PLL_INPUT_MASK=0000 0110
	
	/* CS
	snd_soc_update_bits(codec, AD193X_DAC_CTRL2,
			    AD193X_DAC_WORD_LEN_MASK,
			    word_len << AD193X_DAC_WORD_LEN_SHFT); */

	snd_soc_update_bits(codec, AD193X_ADC_CTRL1,
			    AD193X_ADC_WORD_LEN_MASK, word_len); //ADC_WORD_LEN_MASK=0000 0011

	return 0;
}

static struct snd_soc_dai_ops ad193x_dai_ops = {
	.hw_params = ad193x_hw_params,
	.digital_mute = ad193x_mute,
	.set_tdm_slot = ad193x_set_tdm_slot,
	.set_sysclk	= ad193x_set_dai_sysclk,
	.set_fmt = ad193x_set_dai_fmt,
};

/* codec DAI instance */
static struct snd_soc_dai_driver ad193x_dai = {
	.name = "ad193x-hifi",
	.playback = {
		.stream_name = "Playback",
		.channels_min = 2,
		.channels_max = 8,
		.rates = SNDRV_PCM_RATE_48000,
		.formats = SNDRV_PCM_FMTBIT_S32_LE | SNDRV_PCM_FMTBIT_S16_LE |
			SNDRV_PCM_FMTBIT_S20_3LE | SNDRV_PCM_FMTBIT_S24_LE,
	},
	.capture = {
		.stream_name = "Capture",
		.channels_min = 2,
		.channels_max = 8, //CS: changed from 4 to 8
		.rates = SNDRV_PCM_RATE_48000,
		.formats = SNDRV_PCM_FMTBIT_S32_LE | SNDRV_PCM_FMTBIT_S16_LE |
			SNDRV_PCM_FMTBIT_S20_3LE | SNDRV_PCM_FMTBIT_S24_LE,
	},
	.ops = &ad193x_dai_ops,
};

static int ad193x_probe(struct snd_soc_codec *codec)
{
	struct ad193x_priv *ad193x = snd_soc_codec_get_drvdata(codec);
	struct snd_soc_dapm_context *dapm = &codec->dapm;
	int ret;
	int tmp; //CS

	printk(KERN_DEBUG "Entering ad193x_probe"); //CS

	codec->control_data = ad193x->regmap;
	ret = snd_soc_codec_set_cache_io(codec, 0, 0, SND_SOC_REGMAP);
	if (ret < 0) {
		printk(KERN_DEBUG "Failed to set cache I/O: %d\n", ret); //CS
		dev_err(codec->dev, "failed to set cache I/O: %d\n", ret);
		return ret;
	}

	printk(KERN_DEBUG "writing default values to codec registers"); //CS
	/* default setting for ad193x */

	/* unmute dac channels */
	//snd_soc_write(codec, AD193X_DAC_CHNL_MUTE, 0x0);
	/* de-emphasis: 48kHz, powedown dac */
	//snd_soc_write(codec, AD193X_DAC_CTRL2, 0x1A);
	/* powerdown dac, dac in tdm mode */
	//snd_soc_write(codec, AD193X_DAC_CTRL0, 0x41);
	
	
	/* high-pass filter enable */
	//snd_soc_write(codec, AD193X_ADC_CTRL0, 0x3);
	/* sata delay=1, adc aux mode */
	//snd_soc_write(codec, AD193X_ADC_CTRL1, 0x43);
	/* pll input: mclki/xi */
	//snd_soc_write(codec, AD193X_PLL_CLK_CTRL0, 0x99); /* mclk=24.576Mhz: 0x9D; mclk=12.288Mhz: 0x99 */
	//snd_soc_write(codec, AD193X_PLL_CLK_CTRL1, 0x04); /* disable on-chip voltage reference */

	/*--------------------------------------------*/	
	/*Readback register for debugging-------------*/
	tmp = snd_soc_read(codec, AD193X_PLL_CLK_CTRL0);
	tmp = snd_soc_read(codec, AD193X_PLL_CLK_CTRL1);
	tmp = snd_soc_read(codec, AD193X_AUXPORT_CTRL0);
	tmp = snd_soc_read(codec, AD193X_AUXPORT_CTRL1);
	tmp = snd_soc_read(codec, AD193X_AUXPORT_CTRL2);  
	tmp = snd_soc_read(codec, AD193X_ADC_CTRL0);
	tmp = snd_soc_read(codec, AD193X_ADC_CTRL1);
	tmp = snd_soc_read(codec, AD193X_ADC_CTRL2);
	/*---------------------------------------------*/

	/*Writing default values to registers---------*/
	snd_soc_write(codec, AD193X_PLL_CLK_CTRL0, 0xd8);
		/*   PLL power-down: normal operation [0 (LSB)]
		     MCKLI/XI pin functionality (PLL active): INPUT 256 (x44.1 or 48 kHz) [00]
		   ? MCKLO/XO pin: off (?) [11]
		     PLL input: ALRCLK [10]
		     Internal MCLK enable: Enable->ADC active [1] 
		     ==> 0xd8*/
	snd_soc_write(codec, AD193X_PLL_CLK_CTRL1, 0x04);
		/*   AUXPORT clock source select: PLL clock [0]
		     ADC clock source select: PLL clock [0]
		   ? On-chip voltage reference: disable? [1]
		     PLL lock indicator (READ ONLY!) [x]
		     Reserved [xxxx]
		     ==> 0x04*/
	snd_soc_write(codec, AD193X_AUXPORT_CTRL0, 0x80);
		/*   Reserved [x]
		     Sample rate: 32/44.1/48 kHz [00]
		   ? AUXDATA delay (AUXBLKC periods): 1 (?) [000]
		     Serial format: ADC AUX mode (ADC-,TDM-coupled [10]
		     ==> 0x80*/
	snd_soc_write(codec, AD193X_AUXPORT_CTRL1, 0x70);
		/*   Reserved [x]
		     AUXBCLKs per frame: 64 (two channels) [00]
		     AUXLRCLK polarity: Left low [0]
		     AUXLRCLK master/slave: master [1]
		     AUXBCLK master/slave: master [1]
		     AUXBCLK source: Internally generated [1]
		     AUXBCLK polarity: normal [0]
		     ==> 0x70*/
	snd_soc_write(codec, AD193X_AUXPORT_CTRL2, 0x00);
		/*   Reserved [xxx]
		     Word width: 24 [00]
		     Reserve [xxx]
		     ==> 0x00*/
	snd_soc_write(codec, AD193X_ADC_CTRL0, 0x00);
		/*   Power-down: normal [0]
		     High-pass filter: Off [0]
		     ADC1L/1R/2L/2R mute: Unmute all [0000]
		     Output sample rate: 32/44.1/48 kHz [00]
		     ==> 0x00*/	
	snd_soc_write(codec, AD193X_ADC_CTRL1, 0x40);
		/*   Word width: 24 [00]
		   ? SDATA delay (BCLK periods): 1 [000]
		     Serial format: ADC AUX mode (TDM-coupled) [10]
		     BCLK active edge (TDM_IN): Latch in midcycle (normal) [0]
		     ==> 0x40*/
	snd_soc_write(codec, AD193X_ADC_CTRL2, 0xe4);
		/*   LRCLK format: 50/50 [0]
		     BCLK polarity: Drive out on falling edge (DEF) [0]
		     LRCLK polarity: Left high [1]
		     LRCLK master/slave: slave [0]
		     BCLKs per frame: 256 [10] 
		     BCLK master/slave: master [1]
		     BCLK source: internally generated [1] 
		     ==> 0xe4*/
	/*--------------------------------------------*/	
	/*Readback register for debugging-------------*/
	tmp = snd_soc_read(codec, AD193X_PLL_CLK_CTRL0);
	tmp = snd_soc_read(codec, AD193X_PLL_CLK_CTRL1);
	tmp = snd_soc_read(codec, AD193X_AUXPORT_CTRL0);
	tmp = snd_soc_read(codec, AD193X_AUXPORT_CTRL1);
	tmp = snd_soc_read(codec, AD193X_AUXPORT_CTRL2);  
	tmp = snd_soc_read(codec, AD193X_ADC_CTRL0);
	tmp = snd_soc_read(codec, AD193X_ADC_CTRL1);
	tmp = snd_soc_read(codec, AD193X_ADC_CTRL2);
	/*---------------------------------------------*/
	
	snd_soc_add_controls(codec, ad193x_snd_controls,
			     ARRAY_SIZE(ad193x_snd_controls));
	snd_soc_dapm_new_controls(dapm, ad193x_dapm_widgets,
				  ARRAY_SIZE(ad193x_dapm_widgets));
	snd_soc_dapm_add_routes(dapm, audio_paths, ARRAY_SIZE(audio_paths));

	return ret;
}

static struct snd_soc_codec_driver soc_codec_dev_ad193x = {
	.probe = 	ad193x_probe,
};

#if defined(CONFIG_SPI_MASTER)

static const struct regmap_config ad193x_spi_regmap_config = {
	.val_bits = 8,           //Number of bits in a register value, mandatory.
	.reg_bits = 16,          //Number of bits in a register address, mandatory.
	.read_flag_mask = 0x09,  //Mask to be set in the top byte of the register when doing a read.
	.write_flag_mask = 0x08, //Mask to be set in the top byte of the register when doing a write. 
			         //If both read_flag_mask and write_flag_mask are empty the regmap_bus default masks are used.
};

static int __devinit ad193x_spi_probe(struct spi_device *spi)
{
	struct ad193x_priv *ad193x;
	int ret;

	printk(KERN_DEBUG "Entering: ad193x_spi_probe"); //CS

	ad193x = kzalloc(sizeof(struct ad193x_priv), GFP_KERNEL);
	if (ad193x == NULL)
		return -ENOMEM;

	ad193x->regmap = regmap_init_spi(spi, &ad193x_spi_regmap_config);
	if (IS_ERR(ad193x->regmap)) {
		ret = PTR_ERR(ad193x->regmap);
		goto err_free;
	}

	spi_set_drvdata(spi, ad193x);

	printk(KERN_DEBUG "ad193x.c->ad193x_spi_probe: calling snd_soc_register_codec"); //CS
	ret = snd_soc_register_codec(&spi->dev,
			&soc_codec_dev_ad193x, &ad193x_dai, 1);
	if (ret < 0)
		goto err_regmap_exit;

	printk(KERN_DEBUG "ad193x->ad193x_spi_probe: successful!"); //CS
	return 0;

err_regmap_exit:
	regmap_exit(ad193x->regmap);
err_free:
	kfree(ad193x);

	printk(KERN_DEBUG "ad193x->ad193x_spi_probe: return with error"); //CS
	return ret;
}

static int __devexit ad193x_spi_remove(struct spi_device *spi)
{
	struct ad193x_priv *ad193x = spi_get_drvdata(spi);

	printk(KERN_DEBUG "Entering: ad193x_spi_remove"); //CS

	snd_soc_unregister_codec(&spi->dev);
	regmap_exit(ad193x->regmap);
	kfree(ad193x);
	return 0;
}

static struct spi_driver ad193x_spi_driver = {
	.driver = {
		.name	= "ad193x",
		.owner	= THIS_MODULE,
	},
	.probe		= ad193x_spi_probe,
	.remove		= __devexit_p(ad193x_spi_remove),
};
#endif

#if defined(CONFIG_I2C) || defined(CONFIG_I2C_MODULE)

static const struct regmap_config ad193x_i2c_regmap_config = {
	.val_bits = 8,
	.reg_bits = 8,
};

static const struct i2c_device_id ad193x_id[] = {
	{ "ad1936", 0 },
	{ "ad1937", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, ad193x_id);

static int __devinit ad193x_i2c_probe(struct i2c_client *client,
		const struct i2c_device_id *id)
{
	struct ad193x_priv *ad193x;
	int ret;

	ad193x = kzalloc(sizeof(struct ad193x_priv), GFP_KERNEL);
	if (ad193x == NULL)
		return -ENOMEM;

	ad193x->regmap = regmap_init_i2c(client, &ad193x_i2c_regmap_config);
	if (IS_ERR(ad193x->regmap)) {
		ret = PTR_ERR(ad193x->regmap);
		goto err_free;
	}

	i2c_set_clientdata(client, ad193x);

	ret =  snd_soc_register_codec(&client->dev,
			&soc_codec_dev_ad193x, &ad193x_dai, 1);
	if (ret < 0)
		goto err_regmap_exit;

	return 0;

err_regmap_exit:
	regmap_exit(ad193x->regmap);
err_free:
	kfree(ad193x);
	return ret;
}

static int __devexit ad193x_i2c_remove(struct i2c_client *client)
{
	struct ad193x_priv *ad193x = i2c_get_clientdata(client);

	snd_soc_unregister_codec(&client->dev);
	regmap_exit(ad193x->regmap);
	kfree(ad193x);
	return 0;
}

static struct i2c_driver ad193x_i2c_driver = {
	.driver = {
		.name = "ad193x",
	},
	.probe    = ad193x_i2c_probe,
	.remove   = __devexit_p(ad193x_i2c_remove),
	.id_table = ad193x_id,
};
#endif

static int __init ad193x_modinit(void)
{
	int ret;

#if defined(CONFIG_I2C) || defined(CONFIG_I2C_MODULE)
	/* CS
	printk(KERN_DEBUG "init ad193x_modinit : I2C driver..."); //CS
	ret =  i2c_add_driver(&ad193x_i2c_driver);
	if (ret != 0) {
		printk(KERN_ERR "Failed to register AD193X I2C driver: %d\n",
				ret);
	}
	*/
#endif

#if defined(CONFIG_SPI_MASTER)
	printk(KERN_DEBUG "init ad193x_modinit : SPI driver..."); //CS
	ret = spi_register_driver(&ad193x_spi_driver);
	if (ret != 0) {
		printk(KERN_ERR "Failed to register AD193X SPI driver: %d\n",
				ret);
	} else {
		printk(KERN_DEBUG "AD193X SPI driver registered!");
	}
		
#endif
	return ret;
}
module_init(ad193x_modinit);

static void __exit ad193x_modexit(void)
{
#if defined(CONFIG_SPI_MASTER)
	printk(KERN_DEBUG "ad193x_modexit: spi_unregister_driver"); //CS
	spi_unregister_driver(&ad193x_spi_driver);
#endif

#if defined(CONFIG_I2C) || defined(CONFIG_I2C_MODULE)
	/* CS
	printk(KERN_DEBUG "ad193x_modexit: i2c_del_driver"); //CS
	i2c_del_driver(&ad193x_i2c_driver);
	*/
#endif
}
module_exit(ad193x_modexit);

MODULE_DESCRIPTION("ASoC ad193x driver");
MODULE_AUTHOR("Barry Song <21cnbao@gmail.com>");
MODULE_LICENSE("GPL");
