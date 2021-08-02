#include <platform/mt_pmic.h>
#include <printf.h>

void pmic_lock(void){    
}

void pmic_unlock(void){    
}

void mt6392_upmu_set_rg_vcdt_hv_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCDT_HV_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCDT_HV_EN_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_rgs_chr_ldo_det(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_CHR_CON0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_CHR_LDO_DET_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_CHR_LDO_DET_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_pchr_automode(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_PCHR_AUTOMODE_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_PCHR_AUTOMODE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_csdac_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_CSDAC_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_CSDAC_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_chr_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_CHR_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_CHR_EN_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_rgs_chrdet(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_CHR_CON0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_CHRDET_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_CHRDET_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_vcdt_lv_det(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_CHR_CON0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_VCDT_LV_DET_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_VCDT_LV_DET_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_vcdt_hv_det(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_CHR_CON0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_VCDT_HV_DET_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_VCDT_HV_DET_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_vcdt_lv_vth(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCDT_LV_VTH_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCDT_LV_VTH_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcdt_hv_vth(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCDT_HV_VTH_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCDT_HV_VTH_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vbat_cv_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VBAT_CV_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VBAT_CV_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vbat_cc_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VBAT_CC_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VBAT_CC_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_cs_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_CS_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_CS_EN_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_rgs_cs_det(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_CHR_CON2),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_CS_DET_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_CS_DET_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_vbat_cv_det(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_CHR_CON2),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_VBAT_CV_DET_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_VBAT_CV_DET_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_vbat_cc_det(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_CHR_CON2),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_VBAT_CC_DET_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_VBAT_CC_DET_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_vbat_cv_vth(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VBAT_CV_VTH_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VBAT_CV_VTH_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vbat_cc_vth(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VBAT_CC_VTH_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VBAT_CC_VTH_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_cs_vth(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON4),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_CS_VTH_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_CS_VTH_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_pchr_tohtc(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON5),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_PCHR_TOHTC_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_PCHR_TOHTC_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_pchr_toltc(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON5),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_PCHR_TOLTC_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_PCHR_TOLTC_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vbat_ov_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON6),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VBAT_OV_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VBAT_OV_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vbat_ov_vth(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON6),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VBAT_OV_VTH_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VBAT_OV_VTH_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vbat_ov_deg(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON6),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VBAT_OV_DEG_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VBAT_OV_DEG_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_rgs_vbat_ov_det(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_CHR_CON6),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_VBAT_OV_DET_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_VBAT_OV_DET_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_baton_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_BATON_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_BATON_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_baton_ht_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_BATON_HT_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_BATON_HT_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_baton_tdet_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_BATON_TDET_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_BATON_TDET_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_baton_ht_trim(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_BATON_HT_TRIM_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_BATON_HT_TRIM_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_baton_ht_trim_set(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_BATON_HT_TRIM_SET_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_BATON_HT_TRIM_SET_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_rgs_baton_undet(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_CHR_CON7),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_BATON_UNDET_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_BATON_UNDET_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_csdac_data(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_CSDAC_DATA_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_CSDAC_DATA_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_frc_csvth_usbdl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON9),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_FRC_CSVTH_USBDL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_FRC_CSVTH_USBDL_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_rgs_pchr_flag_out(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_CHR_CON10),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_PCHR_FLAG_OUT_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_PCHR_FLAG_OUT_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_pchr_flag_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_PCHR_FLAG_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_PCHR_FLAG_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_otg_bvalid_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_OTG_BVALID_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_OTG_BVALID_EN_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_rgs_otg_bvalid_det(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_CHR_CON10),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_OTG_BVALID_DET_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_OTG_BVALID_DET_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_pchr_flag_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON11),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_PCHR_FLAG_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_PCHR_FLAG_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_pchr_testmode(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_PCHR_TESTMODE_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_PCHR_TESTMODE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_csdac_testmode(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_CSDAC_TESTMODE_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_CSDAC_TESTMODE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_pchr_rst(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_PCHR_RST_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_PCHR_RST_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_pchr_ft_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_PCHR_FT_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_PCHR_FT_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_chrwdt_td(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON13),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_CHRWDT_TD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_CHRWDT_TD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_chrwdt_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON13),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_CHRWDT_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_CHRWDT_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_chrwdt_wr(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON13),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_CHRWDT_WR_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_CHRWDT_WR_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_pchr_rv(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON14),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_PCHR_RV_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_PCHR_RV_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_chrwdt_int_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON15),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_CHRWDT_INT_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_CHRWDT_INT_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_chrwdt_flag_wr(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON15),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_CHRWDT_FLAG_WR_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_CHRWDT_FLAG_WR_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_rgs_chrwdt_out(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_CHR_CON15),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_CHRWDT_OUT_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_CHRWDT_OUT_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_uvlo_vthl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON16),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_UVLO_VTHL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_UVLO_VTHL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_usbdl_rst(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON16),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_USBDL_RST_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_USBDL_RST_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_usbdl_set(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON16),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_USBDL_SET_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_USBDL_SET_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_adcin_vsen_mux_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON16),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_ADCIN_VSEN_MUX_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_ADCIN_VSEN_MUX_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_adcin_vsen_ext_baton_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON16),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_ADCIN_VSEN_EXT_BATON_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_ADCIN_VSEN_EXT_BATON_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_adcin_vbat_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON16),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_ADCIN_VBAT_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_ADCIN_VBAT_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_adcin_vsen_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON16),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_ADCIN_VSEN_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_ADCIN_VSEN_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_adcin_vchr_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON16),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_ADCIN_VCHR_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_ADCIN_VCHR_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_bgr_rsel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON17),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_BGR_RSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_BGR_RSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_bgr_unchop_ph(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON17),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_BGR_UNCHOP_PH_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_BGR_UNCHOP_PH_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_bgr_unchop(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON17),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_BGR_UNCHOP_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_BGR_UNCHOP_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_bc11_bb_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON18),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_BC11_BB_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_BC11_BB_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_bc11_rst(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON18),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_BC11_RST_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_BC11_RST_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_bc11_vsrc_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON18),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_BC11_VSRC_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_BC11_VSRC_EN_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_rgs_bc11_cmp_out(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_CHR_CON18),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_BC11_CMP_OUT_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_BC11_CMP_OUT_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_dpdm_adcsw_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON18),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_DPDM_ADCSW_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_DPDM_ADCSW_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_dpdm_adcbuf_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON18),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_DPDM_ADCBUF_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_DPDM_ADCBUF_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_buf_dp_in_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON18),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_BUF_DP_IN_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_BUF_DP_IN_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_buf_dm_in_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON18),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_BUF_DM_IN_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_BUF_DM_IN_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vbat_cv_trim(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON18),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VBAT_CV_TRIM_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VBAT_CV_TRIM_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_bc11_vref_vth(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON19),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_BC11_VREF_VTH_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_BC11_VREF_VTH_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_bc11_cmp_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON19),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_BC11_CMP_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_BC11_CMP_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_bc11_ipd_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON19),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_BC11_IPD_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_BC11_IPD_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_bc11_ipu_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON19),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_BC11_IPU_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_BC11_IPU_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_bc11_bias_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON19),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_BC11_BIAS_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_BC11_BIAS_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_csdac_stp_inc(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON20),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_CSDAC_STP_INC_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_CSDAC_STP_INC_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_csdac_stp_dec(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON20),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_CSDAC_STP_DEC_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_CSDAC_STP_DEC_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_csdac_dly(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON21),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_CSDAC_DLY_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_CSDAC_DLY_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_csdac_stp(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON21),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_CSDAC_STP_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_CSDAC_STP_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_low_ich_db(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON22),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_LOW_ICH_DB_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_LOW_ICH_DB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_chrind_on(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON22),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_CHRIND_ON_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_CHRIND_ON_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_chrind_dimming(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON22),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_CHRIND_DIMMING_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_CHRIND_DIMMING_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_cv_mode(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON23),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_CV_MODE_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_CV_MODE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcdt_mode(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON23),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCDT_MODE_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCDT_MODE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_csdac_mode(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON23),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_CSDAC_MODE_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_CSDAC_MODE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_tracking_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON23),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_TRACKING_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_TRACKING_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_hwcv_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON23),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_HWCV_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_HWCV_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_ulc_det_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON23),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_ULC_DET_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_ULC_DET_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_bgr_trim_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON24),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_BGR_TRIM_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_BGR_TRIM_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_ichrg_trim(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON24),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_ICHRG_TRIM_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_ICHRG_TRIM_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_bgr_trim(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON25),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_BGR_TRIM_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_BGR_TRIM_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_ovp_trim(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON26),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_OVP_TRIM_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_OVP_TRIM_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_chr_osc_trim(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON27),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_CHR_OSC_TRIM_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_CHR_OSC_TRIM_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_qi_bgr_ext_buf_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON27),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_QI_BGR_EXT_BUF_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_QI_BGR_EXT_BUF_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_bgr_test_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON27),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_BGR_TEST_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_BGR_TEST_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_bgr_test_rstb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON27),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_BGR_TEST_RSTB_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_BGR_TEST_RSTB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_dac_usbdl_max(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON28),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_DAC_USBDL_MAX_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_DAC_USBDL_MAX_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_pchr_rsv(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON29),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_PCHR_RSV_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_PCHR_RSV_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_thr_det_dis(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_THR_DET_DIS_MASK),
                             (kal_uint32)(MT6392_PMIC_THR_DET_DIS_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_thr_tmode(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_THR_TMODE_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_THR_TMODE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_thr_temp_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_THR_TEMP_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_THR_TEMP_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_strup_thr_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_STRUP_THR_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_STRUP_THR_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_thr_hwpdn_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_THR_HWPDN_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_THR_HWPDN_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_thrdet_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_THRDET_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_THRDET_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_strup_iref_trim(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_STRUP_IREF_TRIM_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_STRUP_IREF_TRIM_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_strup_iref_trim_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_STRUP_IREF_TRIM_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_STRUP_IREF_TRIM_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_usbdl_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_USBDL_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_USBDL_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_fchr_keydet_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_FCHR_KEYDET_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_FCHR_KEYDET_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_fchr_pu_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_FCHR_PU_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_FCHR_PU_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_en_drvsel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EN_DRVSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EN_DRVSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_rst_drvsel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_RST_DRVSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_RST_DRVSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vref_bg(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VREF_BG_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VREF_BG_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_pmu_rsv(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON4),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_PMU_RSV_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_PMU_RSV_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_thr_test(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON5),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_THR_TEST_MASK),
                             (kal_uint32)(MT6392_PMIC_THR_TEST_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_pmu_thr_deb(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_STRUP_CON5),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_PMU_THR_DEB_MASK),
                           (kal_uint32)(MT6392_PMIC_PMU_THR_DEB_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_pmu_thr_status(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_STRUP_CON5),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_PMU_THR_STATUS_MASK),
                           (kal_uint32)(MT6392_PMIC_PMU_THR_STATUS_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_dduvlo_deb_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON6),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_DDUVLO_DEB_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_DDUVLO_DEB_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_pwrbb_deb_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON6),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_PWRBB_DEB_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_PWRBB_DEB_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_strup_osc_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON6),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_STRUP_OSC_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_STRUP_OSC_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_strup_osc_en_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON6),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_STRUP_OSC_EN_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_STRUP_OSC_EN_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_strup_ft_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON6),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_STRUP_FT_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_STRUP_FT_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_strup_pwron_force(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON6),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_STRUP_PWRON_FORCE_MASK),
                             (kal_uint32)(MT6392_PMIC_STRUP_PWRON_FORCE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_bias_gen_en_force(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON6),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_BIAS_GEN_EN_FORCE_MASK),
                             (kal_uint32)(MT6392_PMIC_BIAS_GEN_EN_FORCE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_strup_pwron(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON6),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_STRUP_PWRON_MASK),
                             (kal_uint32)(MT6392_PMIC_STRUP_PWRON_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_strup_pwron_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON6),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_STRUP_PWRON_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_STRUP_PWRON_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_bias_gen_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON6),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_BIAS_GEN_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_BIAS_GEN_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_bias_gen_en_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON6),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_BIAS_GEN_EN_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_BIAS_GEN_EN_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rtc_xosc32_enb_sw(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON6),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RTC_XOSC32_ENB_SW_MASK),
                             (kal_uint32)(MT6392_PMIC_RTC_XOSC32_ENB_SW_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rtc_xosc32_enb_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON6),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RTC_XOSC32_ENB_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RTC_XOSC32_ENB_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_strup_dig_io_pg_force(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON6),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_STRUP_DIG_IO_PG_FORCE_MASK),
                             (kal_uint32)(MT6392_PMIC_STRUP_DIG_IO_PG_FORCE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vadc18_pg_enb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VADC18_PG_ENB_MASK),
                             (kal_uint32)(MT6392_PMIC_VADC18_PG_ENB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcore_pg_enb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCORE_PG_ENB_MASK),
                             (kal_uint32)(MT6392_PMIC_VCORE_PG_ENB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vproc_pg_enb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VPROC_PG_ENB_MASK),
                             (kal_uint32)(MT6392_PMIC_VPROC_PG_ENB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vsys_pg_enb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSYS_PG_ENB_MASK),
                             (kal_uint32)(MT6392_PMIC_VSYS_PG_ENB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vio18_pg_enb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VIO18_PG_ENB_MASK),
                             (kal_uint32)(MT6392_PMIC_VIO18_PG_ENB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vio28_pg_enb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VIO28_PG_ENB_MASK),
                             (kal_uint32)(MT6392_PMIC_VIO28_PG_ENB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vgp2_pg_enb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VGP2_PG_ENB_MASK),
                             (kal_uint32)(MT6392_PMIC_VGP2_PG_ENB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vemc33_pg_enb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VEMC33_PG_ENB_MASK),
                             (kal_uint32)(MT6392_PMIC_VEMC33_PG_ENB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vm25_pg_enb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VM25_PG_ENB_MASK),
                             (kal_uint32)(MT6392_PMIC_VM25_PG_ENB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vm_pg_enb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VM_PG_ENB_MASK),
                             (kal_uint32)(MT6392_PMIC_VM_PG_ENB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vusb_pg_enb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VUSB_PG_ENB_MASK),
                             (kal_uint32)(MT6392_PMIC_VUSB_PG_ENB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vmc_pg_enb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VMC_PG_ENB_MASK),
                             (kal_uint32)(MT6392_PMIC_VMC_PG_ENB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vmch_pg_enb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VMCH_PG_ENB_MASK),
                             (kal_uint32)(MT6392_PMIC_VMCH_PG_ENB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vxo22_pg_enb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VXO22_PG_ENB_MASK),
                             (kal_uint32)(MT6392_PMIC_VXO22_PG_ENB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_strup_con7_rsv0(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_STRUP_CON7_RSV0_MASK),
                             (kal_uint32)(MT6392_PMIC_STRUP_CON7_RSV0_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_clr_just_rst(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_CLR_JUST_RST_MASK),
                             (kal_uint32)(MT6392_PMIC_CLR_JUST_RST_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_uvlo_l2h_deb_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_UVLO_L2H_DEB_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_UVLO_L2H_DEB_EN_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_just_pwrkey_rst(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_STRUP_CON8),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_JUST_PWRKEY_RST_MASK),
                           (kal_uint32)(MT6392_PMIC_JUST_PWRKEY_RST_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_qi_osc_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_STRUP_CON8),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_QI_OSC_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_QI_OSC_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_strup_ext_pmic_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON9),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_STRUP_EXT_PMIC_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_STRUP_EXT_PMIC_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_strup_ext_pmic_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON9),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_STRUP_EXT_PMIC_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_STRUP_EXT_PMIC_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_strup_con8_rsv0(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON9),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_STRUP_CON8_RSV0_MASK),
                             (kal_uint32)(MT6392_PMIC_STRUP_CON8_RSV0_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_qi_ext_pmic_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_STRUP_CON9),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_QI_EXT_PMIC_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_QI_EXT_PMIC_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_strup_auxadc_start_sw(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_STRUP_AUXADC_START_SW_MASK),
                             (kal_uint32)(MT6392_PMIC_STRUP_AUXADC_START_SW_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_strup_auxadc_rstb_sw(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_STRUP_AUXADC_RSTB_SW_MASK),
                             (kal_uint32)(MT6392_PMIC_STRUP_AUXADC_RSTB_SW_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_strup_auxadc_start_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_STRUP_AUXADC_START_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_STRUP_AUXADC_START_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_strup_auxadc_rstb_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_STRUP_AUXADC_RSTB_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_STRUP_AUXADC_RSTB_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_strup_auxadc_rpcnt_max(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_STRUP_AUXADC_RPCNT_MAX_MASK),
                             (kal_uint32)(MT6392_PMIC_STRUP_AUXADC_RPCNT_MAX_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_strup_pwroff_seq_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON11),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_STRUP_PWROFF_SEQ_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_STRUP_PWROFF_SEQ_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_strup_pwroff_preoff_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON11),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_STRUP_PWROFF_PREOFF_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_STRUP_PWROFF_PREOFF_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_spk_en_l(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_SPK_EN_L_MASK),
                             (kal_uint32)(MT6392_PMIC_SPK_EN_L_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_spkmode_l(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_SPKMODE_L_MASK),
                             (kal_uint32)(MT6392_PMIC_SPKMODE_L_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_spk_trim_en_l(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_SPK_TRIM_EN_L_MASK),
                             (kal_uint32)(MT6392_PMIC_SPK_TRIM_EN_L_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_spk_oc_shdn_dl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_SPK_OC_SHDN_DL_MASK),
                             (kal_uint32)(MT6392_PMIC_SPK_OC_SHDN_DL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_spk_ther_shdn_l_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_SPK_THER_SHDN_L_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_SPK_THER_SHDN_L_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_spk_out_stage_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_SPK_OUT_STAGE_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_SPK_OUT_STAGE_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spk_gainl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_GAINL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_GAINL_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_spk_offset_l(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_SPK_CON1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_SPK_OFFSET_L_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_SPK_OFFSET_L_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_da_spk_lead_dglh_l(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_SPK_CON1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_SPK_LEAD_DGLH_L_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_SPK_LEAD_DGLH_L_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_ad_spk_lead_l(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_SPK_CON1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AD_SPK_LEAD_L_MASK),
                           (kal_uint32)(MT6392_PMIC_AD_SPK_LEAD_L_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_spk_offset_l_ov(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_SPK_CON1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_SPK_OFFSET_L_OV_MASK),
                           (kal_uint32)(MT6392_PMIC_SPK_OFFSET_L_OV_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_spk_offset_l_sw(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_SPK_OFFSET_L_SW_MASK),
                             (kal_uint32)(MT6392_PMIC_SPK_OFFSET_L_SW_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_spk_lead_l_sw(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_SPK_LEAD_L_SW_MASK),
                             (kal_uint32)(MT6392_PMIC_SPK_LEAD_L_SW_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_spk_offset_l_mode(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_SPK_OFFSET_L_MODE_MASK),
                             (kal_uint32)(MT6392_PMIC_SPK_OFFSET_L_MODE_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_spk_trim_done_l(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_SPK_CON1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_SPK_TRIM_DONE_L_MASK),
                           (kal_uint32)(MT6392_PMIC_SPK_TRIM_DONE_L_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_spk_intg_rst_l(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_INTG_RST_L_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_INTG_RST_L_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spk_force_en_l(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_FORCE_EN_L_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_FORCE_EN_L_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spk_slew_l(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_SLEW_L_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_SLEW_L_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spkab_obias_l(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPKAB_OBIAS_L_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPKAB_OBIAS_L_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spkrcv_en_l(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPKRCV_EN_L_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPKRCV_EN_L_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spk_drc_en_l(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_DRC_EN_L_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_DRC_EN_L_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spk_test_en_l(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_TEST_EN_L_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_TEST_EN_L_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spkab_oc_en_l(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPKAB_OC_EN_L_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPKAB_OC_EN_L_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spk_oc_en_l(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_OC_EN_L_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_OC_EN_L_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_spk_trim_wnd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON6),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_SPK_TRIM_WND_MASK),
                             (kal_uint32)(MT6392_PMIC_SPK_TRIM_WND_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_spk_trim_thd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON6),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_SPK_TRIM_THD_MASK),
                             (kal_uint32)(MT6392_PMIC_SPK_TRIM_THD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_spk_oc_wnd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON6),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_SPK_OC_WND_MASK),
                             (kal_uint32)(MT6392_PMIC_SPK_OC_WND_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_spk_oc_thd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON6),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_SPK_OC_THD_MASK),
                             (kal_uint32)(MT6392_PMIC_SPK_OC_THD_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_spk_d_oc_l_deg(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_SPK_CON6),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_SPK_D_OC_L_DEG_MASK),
                           (kal_uint32)(MT6392_PMIC_SPK_D_OC_L_DEG_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_spk_ab_oc_l_deg(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_SPK_CON6),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_SPK_AB_OC_L_DEG_MASK),
                           (kal_uint32)(MT6392_PMIC_SPK_AB_OC_L_DEG_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_spk_td1(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_SPK_TD1_MASK),
                             (kal_uint32)(MT6392_PMIC_SPK_TD1_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_spk_td2(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_SPK_TD2_MASK),
                             (kal_uint32)(MT6392_PMIC_SPK_TD2_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_spk_td3(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_SPK_TD3_MASK),
                             (kal_uint32)(MT6392_PMIC_SPK_TD3_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_spk_trim_div(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_SPK_TRIM_DIV_MASK),
                             (kal_uint32)(MT6392_PMIC_SPK_TRIM_DIV_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_btl_set(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_BTL_SET_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_BTL_SET_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spk_ibias_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_IBIAS_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_IBIAS_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spk_ccode(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_CCODE_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_CCODE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spk_en_view_vcm(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_EN_VIEW_VCM_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_EN_VIEW_VCM_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spk_en_view_clk(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_EN_VIEW_CLK_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_EN_VIEW_CLK_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spk_vcm_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_VCM_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_VCM_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spk_vcm_ibsel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_VCM_IBSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_VCM_IBSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spk_fbrc_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_FBRC_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_FBRC_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spkab_ovdrv(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPKAB_OVDRV_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPKAB_OVDRV_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spk_octh_d(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_OCTH_D_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_OCTH_D_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spk_rsv(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON9),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_RSV_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_RSV_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spkpga_gain(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON9),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPKPGA_GAIN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPKPGA_GAIN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_spk_rsv0(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON9),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_SPK_RSV0_MASK),
                             (kal_uint32)(MT6392_PMIC_SPK_RSV0_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_spk_vcm_fast_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON9),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_SPK_VCM_FAST_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_SPK_VCM_FAST_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_spk_test_mode0(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON9),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_SPK_TEST_MODE0_MASK),
                             (kal_uint32)(MT6392_PMIC_SPK_TEST_MODE0_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_spk_test_mode1(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON9),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_SPK_TEST_MODE1_MASK),
                             (kal_uint32)(MT6392_PMIC_SPK_TEST_MODE1_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spk_isense_refsel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_ISENSE_REFSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_ISENSE_REFSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spk_isense_gainsel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_ISENSE_GAINSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_ISENSE_GAINSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spk_isense_pdreset(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_ISENSE_PDRESET_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_ISENSE_PDRESET_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spk_isense_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_ISENSE_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_ISENSE_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spk_isense_test_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_ISENSE_TEST_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_ISENSE_TEST_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_spk_td_wait(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON11),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_SPK_TD_WAIT_MASK),
                             (kal_uint32)(MT6392_PMIC_SPK_TD_WAIT_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_spk_td_done(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON11),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_SPK_TD_DONE_MASK),
                             (kal_uint32)(MT6392_PMIC_SPK_TD_DONE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_spk_en_mode(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_SPK_EN_MODE_MASK),
                             (kal_uint32)(MT6392_PMIC_SPK_EN_MODE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_spk_vcm_fast_sw(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_SPK_VCM_FAST_SW_MASK),
                             (kal_uint32)(MT6392_PMIC_SPK_VCM_FAST_SW_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_spk_rst_l_sw(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_SPK_RST_L_SW_MASK),
                             (kal_uint32)(MT6392_PMIC_SPK_RST_L_SW_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_spkmode_l_sw(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_SPKMODE_L_SW_MASK),
                             (kal_uint32)(MT6392_PMIC_SPKMODE_L_SW_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_spk_depop_en_l_sw(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_SPK_DEPOP_EN_L_SW_MASK),
                             (kal_uint32)(MT6392_PMIC_SPK_DEPOP_EN_L_SW_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_spk_en_l_sw(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_SPK_EN_L_SW_MASK),
                             (kal_uint32)(MT6392_PMIC_SPK_EN_L_SW_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_spk_outstg_en_l_sw(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_SPK_OUTSTG_EN_L_SW_MASK),
                             (kal_uint32)(MT6392_PMIC_SPK_OUTSTG_EN_L_SW_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_spk_trim_en_l_sw(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_SPK_TRIM_EN_L_SW_MASK),
                             (kal_uint32)(MT6392_PMIC_SPK_TRIM_EN_L_SW_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_spk_trim_stop_l_sw(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SPK_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_SPK_TRIM_STOP_L_SW_MASK),
                             (kal_uint32)(MT6392_PMIC_SPK_TRIM_STOP_L_SW_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vadc18_pg_h2l_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VADC18_PG_H2L_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_VADC18_PG_H2L_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcore_pg_h2l_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCORE_PG_H2L_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_VCORE_PG_H2L_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vproc_pg_h2l_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VPROC_PG_H2L_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_VPROC_PG_H2L_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vsys_pg_h2l_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSYS_PG_H2L_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_VSYS_PG_H2L_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vio18_pg_h2l_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VIO18_PG_H2L_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_VIO18_PG_H2L_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vio28_pg_h2l_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VIO28_PG_H2L_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_VIO28_PG_H2L_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vgp2_pg_h2l_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VGP2_PG_H2L_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_VGP2_PG_H2L_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vemc33_pg_h2l_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VEMC33_PG_H2L_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_VEMC33_PG_H2L_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vm25_pg_h2l_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VM25_PG_H2L_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_VM25_PG_H2L_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vm_pg_h2l_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VM_PG_H2L_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_VM_PG_H2L_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vusb_pg_h2l_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VUSB_PG_H2L_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_VUSB_PG_H2L_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vmc_pg_h2l_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VMC_PG_H2L_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_VMC_PG_H2L_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vmch_pg_h2l_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VMCH_PG_H2L_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_VMCH_PG_H2L_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vxo22_pg_h2l_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VXO22_PG_H2L_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_VXO22_PG_H2L_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_strup_con12_rsv0(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_STRUP_CON12_RSV0_MASK),
                             (kal_uint32)(MT6392_PMIC_STRUP_CON12_RSV0_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_strup_long_press_ext_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON13),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_STRUP_LONG_PRESS_EXT_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_STRUP_LONG_PRESS_EXT_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_strup_long_press_ext_td(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON13),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_STRUP_LONG_PRESS_EXT_TD_MASK),
                             (kal_uint32)(MT6392_PMIC_STRUP_LONG_PRESS_EXT_TD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_strup_long_press_ext_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON13),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_STRUP_LONG_PRESS_EXT_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_STRUP_LONG_PRESS_EXT_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_strup_long_press_ext_chr_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON13),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_STRUP_LONG_PRESS_EXT_CHR_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_STRUP_LONG_PRESS_EXT_CHR_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_strup_long_press_ext_pwrkey_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON13),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_STRUP_LONG_PRESS_EXT_PWRKEY_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_STRUP_LONG_PRESS_EXT_PWRKEY_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_strup_long_press_ext_pwrbb_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON13),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_STRUP_LONG_PRESS_EXT_PWRBB_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_STRUP_LONG_PRESS_EXT_PWRBB_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_strup_pwrkey_count_reset(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON14),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_STRUP_PWRKEY_COUNT_RESET_MASK),
                             (kal_uint32)(MT6392_PMIC_STRUP_PWRKEY_COUNT_RESET_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_strup_long_press_reset_extend(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON15),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_STRUP_LONG_PRESS_RESET_EXTEND_MASK),
                             (kal_uint32)(MT6392_PMIC_STRUP_LONG_PRESS_RESET_EXTEND_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcore_oc_pdn_enb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON16),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCORE_OC_PDN_ENB_MASK),
                             (kal_uint32)(MT6392_PMIC_VCORE_OC_PDN_ENB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vproc_oc_pdn_enb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON16),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VPROC_OC_PDN_ENB_MASK),
                             (kal_uint32)(MT6392_PMIC_VPROC_OC_PDN_ENB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vsys_oc_pdn_enb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON16),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSYS_OC_PDN_ENB_MASK),
                             (kal_uint32)(MT6392_PMIC_VSYS_OC_PDN_ENB_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_rgs_vadc18_pg_pwroff(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_STRUP_CON18),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_VADC18_PG_PWROFF_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_VADC18_PG_PWROFF_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_vcore_pg_pwroff(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_STRUP_CON18),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_VCORE_PG_PWROFF_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_VCORE_PG_PWROFF_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_vproc_pg_pwroff(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_STRUP_CON18),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_VPROC_PG_PWROFF_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_VPROC_PG_PWROFF_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_vsys_pg_pwroff(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_STRUP_CON18),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_VSYS_PG_PWROFF_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_VSYS_PG_PWROFF_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_vio18_pg_pwroff(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_STRUP_CON18),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_VIO18_PG_PWROFF_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_VIO18_PG_PWROFF_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_vio28_pg_pwroff(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_STRUP_CON18),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_VIO28_PG_PWROFF_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_VIO28_PG_PWROFF_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_vgp2_pg_pwroff(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_STRUP_CON18),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_VGP2_PG_PWROFF_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_VGP2_PG_PWROFF_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_vemc33_pg_pwroff(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_STRUP_CON18),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_VEMC33_PG_PWROFF_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_VEMC33_PG_PWROFF_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_vm25_pg_pwroff(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_STRUP_CON18),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_VM25_PG_PWROFF_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_VM25_PG_PWROFF_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_vm_pg_pwroff(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_STRUP_CON18),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_VM_PG_PWROFF_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_VM_PG_PWROFF_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_vusb_pg_pwroff(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_STRUP_CON18),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_VUSB_PG_PWROFF_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_VUSB_PG_PWROFF_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_vmc_pg_pwroff(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_STRUP_CON18),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_VMC_PG_PWROFF_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_VMC_PG_PWROFF_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_vmch_pg_pwroff(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_STRUP_CON18),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_VMCH_PG_PWROFF_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_VMCH_PG_PWROFF_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_vxo22_pg_pwroff(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_STRUP_CON18),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_VXO22_PG_PWROFF_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_VXO22_PG_PWROFF_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_vcore_oc_pwroff(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_STRUP_CON19),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_VCORE_OC_PWROFF_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_VCORE_OC_PWROFF_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_vproc_oc_pwroff(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_STRUP_CON19),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_VPROC_OC_PWROFF_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_VPROC_OC_PWROFF_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_vsys_oc_pwroff(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_STRUP_CON19),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_VSYS_OC_PWROFF_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_VSYS_OC_PWROFF_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_uvlo_pwroff(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_STRUP_CON19),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_UVLO_PWROFF_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_UVLO_PWROFF_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_thr_pwroff(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_STRUP_CON19),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_THR_PWROFF_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_THR_PWROFF_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_long_press_pwroff(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_STRUP_CON19),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_LONG_PRESS_PWROFF_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_LONG_PRESS_PWROFF_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_wdtrst_pwroff(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_STRUP_CON19),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_WDTRST_PWROFF_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_WDTRST_PWROFF_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_rtc_pwrbb_pwroff(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_STRUP_CON19),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_RTC_PWRBB_PWROFF_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_RTC_PWRBB_PWROFF_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_ddlo_pwroff(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_STRUP_CON19),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_DDLO_PWROFF_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_DDLO_PWROFF_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_pwrcond_pwroff(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_STRUP_CON19),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_PWRCOND_PWROFF_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_PWRCOND_PWROFF_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_strup_pwroff_hw_clr_enb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON19),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_STRUP_PWROFF_HW_CLR_ENB_MASK),
                             (kal_uint32)(MT6392_PMIC_STRUP_PWROFF_HW_CLR_ENB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_strup_swrsv(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_STRUP_CON20),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_STRUP_SWRSV_MASK),
                             (kal_uint32)(MT6392_PMIC_STRUP_SWRSV_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_cid(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_CID),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_CID_MASK),
                           (kal_uint32)(MT6392_PMIC_CID_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_clksq_en_monen(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_CLKSQ_EN_MONEN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_CLKSQ_EN_MONEN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_clksq_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_CLKSQ_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_CLKSQ_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_strup_75k_div2_ck_pdn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_STRUP_75K_DIV2_CK_PDN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_STRUP_75K_DIV2_CK_PDN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_strup_75k_ck_pdn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_STRUP_75K_CK_PDN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_STRUP_75K_CK_PDN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_ckpdn0_rsv_4(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_CKPDN0_RSV_4_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_CKPDN0_RSV_4_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_rtc_75k_div4_ck_pdn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_RTC_75K_DIV4_CK_PDN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_RTC_75K_DIV4_CK_PDN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_rtc_75k_ck_pdn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_RTC_75K_CK_PDN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_RTC_75K_CK_PDN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_rtc_32k_ck_pdn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_RTC_32K_CK_PDN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_RTC_32K_CK_PDN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_pchr_32k_ck_pdn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_PCHR_32K_CK_PDN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_PCHR_32K_CK_PDN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_ldostb_1m_ck_pdn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_LDOSTB_1M_CK_PDN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_LDOSTB_1M_CK_PDN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_intrp_ck_pdn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_INTRP_CK_PDN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_INTRP_CK_PDN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_trim_75k_ck_pdn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_TRIM_75K_CK_PDN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_TRIM_75K_CK_PDN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_buck_1m_ck_pdn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_BUCK_1M_CK_PDN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_BUCK_1M_CK_PDN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_buck_ck_pdn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_BUCK_CK_PDN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_BUCK_CK_PDN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_buck_ana_ck_pdn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_BUCK_ANA_CK_PDN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_BUCK_ANA_CK_PDN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_buck32k_pdn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_BUCK32K_PDN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_BUCK32K_PDN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_strup_6m_pdn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_STRUP_6M_PDN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_STRUP_6M_PDN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spk_pwm_div_pdn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_PWM_DIV_PDN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_PWM_DIV_PDN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spk_div_pdn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_DIV_PDN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_DIV_PDN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spk_ck_pdn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_CK_PDN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_CK_PDN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_pwmoc_ck_pdn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_PWMOC_CK_PDN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_PWMOC_CK_PDN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_fqmtr_pdn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_FQMTR_PDN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_FQMTR_PDN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_fqmtr_32k_ck_pdn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_FQMTR_32K_CK_PDN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_FQMTR_32K_CK_PDN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_top_ckpdn1_rsv_7(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKPDN1_RSV_7_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKPDN1_RSV_7_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_top_ckpdn1_rsv_8(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKPDN1_RSV_8_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKPDN1_RSV_8_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_rtcdet_ck_pdn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_RTCDET_CK_PDN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_RTCDET_CK_PDN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_rtc_mclk_pdn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_RTC_MCLK_PDN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_RTC_MCLK_PDN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_smps_ck_div_pdn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SMPS_CK_DIV_PDN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SMPS_CK_DIV_PDN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_ck_pdn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_CK_PDN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_CK_PDN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_rtc32k_1v8_pdn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_RTC32K_1V8_PDN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_RTC32K_1V8_PDN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_ckpdn1_rsv_15_14(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_CKPDN1_RSV_15_14_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_CKPDN1_RSV_15_14_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_top_ckpdn2_rsv_0(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKPDN2_RSV_0_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKPDN2_RSV_0_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_top_ckpdn2_rsv_1(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKPDN2_RSV_1_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKPDN2_RSV_1_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_top_ckpdn2_rsv_2(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKPDN2_RSV_2_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKPDN2_RSV_2_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_auxadc_rng_ck_pdn_hwen(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_AUXADC_RNG_CK_PDN_HWEN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_AUXADC_RNG_CK_PDN_HWEN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_auxadc_ck_pdn_hwen(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_AUXADC_CK_PDN_HWEN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_AUXADC_CK_PDN_HWEN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_auxadc_ck_pdn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_AUXADC_CK_PDN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_AUXADC_CK_PDN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_auxadc_1m_ck_pdn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_AUXADC_1M_CK_PDN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_AUXADC_1M_CK_PDN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_top_ckpdn2_rsv_7(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKPDN2_RSV_7_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKPDN2_RSV_7_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_type_c_csr_ck_pdn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_TYPE_C_CSR_CK_PDN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_TYPE_C_CSR_CK_PDN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_type_c_cc_ck_pdn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_TYPE_C_CC_CK_PDN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_TYPE_C_CC_CK_PDN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_auxadc_32k_ck_pdn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_AUXADC_32K_CK_PDN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_AUXADC_32K_CK_PDN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_auxadc_rng_ck_pdn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_AUXADC_RNG_CK_PDN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_AUXADC_RNG_CK_PDN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_auxadc_rng_hw_mode(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKPDN2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_AUXADC_RNG_HW_MODE_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_AUXADC_RNG_HW_MODE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_man_rst(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_RST_CON),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_MAN_RST_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_MAN_RST_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_auxadc_rst(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_RST_CON),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_AUXADC_RST_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_AUXADC_RST_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_rst_con_rsv_2(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_RST_CON),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_RST_CON_RSV_2_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_RST_CON_RSV_2_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_accdet_rst(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_RST_CON),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_ACCDET_RST_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_ACCDET_RST_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spk_rst(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_RST_CON),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_RST_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_RST_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_rst_con_rsv_6(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_RST_CON),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_RST_CON_RSV_6_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_RST_CON_RSV_6_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_rtc_rst(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_RST_CON),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_RTC_RST_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_RTC_RST_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_fqmtr_rst(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_RST_CON),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_FQMTR_RST_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_FQMTR_RST_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_auxadc_reg_rst(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_RST_CON),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_AUXADC_REG_RST_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_AUXADC_REG_RST_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_type_c_cc_rst(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_RST_CON),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_TYPE_C_CC_RST_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_TYPE_C_CC_RST_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_clk_trim_rst(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_RST_CON),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_CLK_TRIM_RST_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_CLK_TRIM_RST_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_strup_long_press_rst(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_RST_CON),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_STRUP_LONG_PRESS_RST_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_STRUP_LONG_PRESS_RST_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_top_rst_con_rsv_15_13(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_RST_CON),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_RST_CON_RSV_15_13_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_RST_CON_RSV_15_13_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_ap_rst_dis(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_RST_MISC),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_AP_RST_DIS_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_AP_RST_DIS_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_sysrstb_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_RST_MISC),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SYSRSTB_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SYSRSTB_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_strup_man_rst_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_RST_MISC),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_STRUP_MAN_RST_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_STRUP_MAN_RST_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_newldo_rstb_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_RST_MISC),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_NEWLDO_RSTB_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_NEWLDO_RSTB_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_rst_part_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_RST_MISC),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_RST_PART_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_RST_PART_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_homekey_rst_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_RST_MISC),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_HOMEKEY_RST_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_HOMEKEY_RST_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_pwrkey_rst_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_RST_MISC),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_PWRKEY_RST_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_PWRKEY_RST_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_pwrrst_tmr_dis(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_RST_MISC),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_PWRRST_TMR_DIS_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_PWRRST_TMR_DIS_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_pwrkey_rst_td(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_RST_MISC),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_PWRKEY_RST_TD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_PWRKEY_RST_TD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_long_press_ext_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_RST_MISC),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_LONG_PRESS_EXT_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_LONG_PRESS_EXT_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_rst_misc_rsv_15_11(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_RST_MISC),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_RST_MISC_RSV_15_11_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_RST_MISC_RSV_15_11_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_srclken_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKCON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SRCLKEN_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SRCLKEN_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_osc_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKCON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_OSC_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_OSC_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_top_ckcon0_rsv_3(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKCON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKCON0_RSV_3_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKCON0_RSV_3_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_srclken_hw_mode(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKCON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SRCLKEN_HW_MODE_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SRCLKEN_HW_MODE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_osc_hw_mode(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKCON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_OSC_HW_MODE_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_OSC_HW_MODE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_osc_hw_src_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKCON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_OSC_HW_SRC_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_OSC_HW_SRC_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_auxadc_ck_hw_mode(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKCON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_AUXADC_CK_HW_MODE_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_AUXADC_CK_HW_MODE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_smps_autoff_dis(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKCON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SMPS_AUTOFF_DIS_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SMPS_AUTOFF_DIS_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_buck_1m_autoff_dis(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKCON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_BUCK_1M_AUTOFF_DIS_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_BUCK_1M_AUTOFF_DIS_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_auxadc_ck_src_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKCON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_AUXADC_CK_SRC_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_AUXADC_CK_SRC_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_top_ckcon0_rsv_14_12(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKCON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKCON0_RSV_14_12_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKCON0_RSV_14_12_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_buck_ana_autoff_dis(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKCON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_BUCK_ANA_AUTOFF_DIS_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_BUCK_ANA_AUTOFF_DIS_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_regck_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKCON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_REGCK_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_REGCK_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spk_pwm_div_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKCON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_PWM_DIV_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_PWM_DIV_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spk_div_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKCON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_DIV_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_DIV_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_fqmtr_cksel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKCON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_FQMTR_CKSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_FQMTR_CKSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_top_ckcon1_rsv_9(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKCON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKCON1_RSV_9_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKCON1_RSV_9_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_top_ckcon1_rsv_10(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKCON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKCON1_RSV_10_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKCON1_RSV_10_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_top_ckcon1_rsv_11(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKCON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKCON1_RSV_11_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKCON1_RSV_11_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_top_ckcon1_rsv_12(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKCON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKCON1_RSV_12_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKCON1_RSV_12_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_top_ckcon1_rsv_13(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKCON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKCON1_RSV_13_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKCON1_RSV_13_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_top_ckcon1_rsv_14(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKCON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKCON1_RSV_14_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKCON1_RSV_14_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_top_ckcon1_rsv_15(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKCON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKCON1_RSV_15_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKCON1_RSV_15_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_rtc32k_tst_dis(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKTST0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_RTC32K_TST_DIS_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_RTC32K_TST_DIS_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spk_tst_dis(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKTST0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_TST_DIS_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_TST_DIS_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_smps_tst_dis(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKTST0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SMPS_TST_DIS_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SMPS_TST_DIS_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_pmu75k_tst_dis(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKTST0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_PMU75K_TST_DIS_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_PMU75K_TST_DIS_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_aud26m_tst_dis(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKTST0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_AUD26M_TST_DIS_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_AUD26M_TST_DIS_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spk_tstsel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKTST1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_TSTSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPK_TSTSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_smps_tstsel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKTST1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SMPS_TSTSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SMPS_TSTSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_rtc32k_tstsel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKTST1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_RTC32K_TSTSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_RTC32K_TSTSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_pmu75k_tstsel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKTST1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_PMU75K_TSTSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_PMU75K_TSTSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_aud26m_tstsel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKTST1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_AUD26M_TSTSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_AUD26M_TSTSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_rtcdet_tstsel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKTST1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_RTCDET_TSTSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_RTCDET_TSTSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_pwmoc_tstsel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKTST1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_PWMOC_TSTSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_PWMOC_TSTSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_ldostb_tstsel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKTST1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_LDOSTB_TSTSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_LDOSTB_TSTSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_auxadc_ck_tstsel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKTST1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_AUXADC_CK_TSTSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_AUXADC_CK_TSTSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_fqmtr_tstsel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKTST1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_FQMTR_TSTSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_FQMTR_TSTSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_classd_tstsel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKTST1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_CLASSD_TSTSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_CLASSD_TSTSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_top_cktst1_rsv_11(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKTST1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKTST1_RSV_11_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKTST1_RSV_11_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_top_cktst1_rsv_12(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKTST1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKTST1_RSV_12_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKTST1_RSV_12_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_top_cktst1_rsv_13(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKTST1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKTST1_RSV_13_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKTST1_RSV_13_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_bgr_test_ck_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKTST2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_BGR_TEST_CK_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_BGR_TEST_CK_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_pchr_test_ck_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKTST2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_PCHR_TEST_CK_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_PCHR_TEST_CK_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_strup_75k_26m_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKTST2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_STRUP_75K_26M_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_STRUP_75K_26M_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_bgr_testmode(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKTST2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_BGR_TESTMODE_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_BGR_TESTMODE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_32k_75k_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKTST2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_32K_75K_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_32K_75K_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_top_cktst2_rsv_15_8(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TOP_CKTST2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKTST2_RSV_15_8_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_TOP_CKTST2_RSV_15_8_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_test_out(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_TEST_OUT),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TEST_OUT_MASK),
                           (kal_uint32)(MT6392_PMIC_TEST_OUT_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_mon_flag_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TEST_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_MON_FLAG_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_MON_FLAG_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_mon_grp_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TEST_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_MON_GRP_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_MON_GRP_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_wdtrstb_deb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TEST_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_WDTRSTB_DEB_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_WDTRSTB_DEB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_test_driver(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TEST_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_TEST_DRIVER_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_TEST_DRIVER_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_test_classd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TEST_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_TEST_CLASSD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_TEST_CLASSD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_test_aud(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TEST_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_TEST_AUD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_TEST_AUD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_test_auxadc(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TEST_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_TEST_AUXADC_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_TEST_AUXADC_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_nandtree_mode(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TEST_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_NANDTREE_MODE_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_NANDTREE_MODE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_mode(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TEST_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_MODE_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_MODE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_test_strup(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TEST_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_TEST_STRUP_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_TEST_STRUP_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_test_spk(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TEST_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_TEST_SPK_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_TEST_SPK_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_test_spk_pwm(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TEST_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_TEST_SPK_PWM_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_TEST_SPK_PWM_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_top_en_status_vproc(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EN_STATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_EN_STATUS_VPROC_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_EN_STATUS_VPROC_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_en_status_vsys(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EN_STATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_EN_STATUS_VSYS_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_EN_STATUS_VSYS_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_en_status_vcore(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EN_STATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_EN_STATUS_VCORE_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_EN_STATUS_VCORE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_da_qi_vaud28_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EN_STATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VAUD28_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VAUD28_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_da_qi_vtcxo_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EN_STATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VTCXO_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VTCXO_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_da_qi_vaud22_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EN_STATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VAUD22_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VAUD22_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_da_qi_vadc18_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EN_STATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VADC18_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VADC18_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_da_qi_vcama_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EN_STATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VCAMA_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VCAMA_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_da_qi_vcn35_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EN_STATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VCN35_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VCN35_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_da_qi_vio28_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EN_STATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VIO28_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VIO28_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_da_qi_vusb_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EN_STATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VUSB_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VUSB_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_da_qi_vmc_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EN_STATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VMC_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VMC_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_da_qi_vmch_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EN_STATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VMCH_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VMCH_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_da_qi_vemc_3v3_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EN_STATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VEMC_3V3_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VEMC_3V3_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_da_qi_vcam_af_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EN_STATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VCAM_AF_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VCAM_AF_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_da_qi_vgp1_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EN_STATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VGP1_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VGP1_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_da_qi_vgp2_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EN_STATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VGP2_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VGP2_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_da_qi_vm25_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EN_STATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VM25_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VM25_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_da_qi_vefuse_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EN_STATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VEFUSE_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VEFUSE_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_da_qi_vm_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EN_STATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VM_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VM_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_da_qi_vcn_1v8_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EN_STATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VCN_1V8_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VCN_1V8_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_da_qi_vcamd_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EN_STATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VCAMD_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VCAMD_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_da_qi_vcam_io_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EN_STATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VCAM_IO_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VCAM_IO_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_da_qi_vrtc_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EN_STATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VRTC_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VRTC_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_da_qi_vio18_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EN_STATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VIO18_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VIO18_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_da_qi_vadc18_stb(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EN_STATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VADC18_STB_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VADC18_STB_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_da_qi_vm25_stb(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EN_STATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VM25_STB_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_DA_QI_VM25_STB_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_oc_status_vproc(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_OCSTATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_OC_STATUS_VPROC_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_OC_STATUS_VPROC_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_oc_status_vsys(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_OCSTATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_OC_STATUS_VSYS_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_OC_STATUS_VSYS_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_oc_status_vcore(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_OCSTATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_OC_STATUS_VCORE_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_OC_STATUS_VCORE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_rgs_qi_vcn35_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_OCSTATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VCN35_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VCN35_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_rgs_qi_vio28_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_OCSTATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VIO28_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VIO28_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_rgs_qi_vusb_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_OCSTATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VUSB_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VUSB_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_rgs_qi_vmc_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_OCSTATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VMC_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VMC_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_rgs_qi_vmch_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_OCSTATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VMCH_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VMCH_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_rgs_qi_vemc_3v3_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_OCSTATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VEMC_3V3_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VEMC_3V3_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_rgs_qi_vcam_af_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_OCSTATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VCAM_AF_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VCAM_AF_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_rgs_qi_vgp1_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_OCSTATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VGP1_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VGP1_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_rgs_qi_vgp2_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_OCSTATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VGP2_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VGP2_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_rgs_qi_vm25_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_OCSTATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VM25_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VM25_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_rgs_qi_vefuse_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_OCSTATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VEFUSE_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VEFUSE_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_rgs_qi_vm_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_OCSTATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VM_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VM_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_rgs_qi_vio18_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_OCSTATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VIO18_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VIO18_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_rgs_qi_vcn_1v8_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_OCSTATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VCN_1V8_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VCN_1V8_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_rgs_qi_vcamd_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_OCSTATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VCAMD_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VCAMD_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_rgs_qi_vcam_io_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_OCSTATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VCAM_IO_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VCAM_IO_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_rgs_qi_vtcxo_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_OCSTATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VTCXO_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VTCXO_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_rgs_qi_vadc18_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_OCSTATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VADC18_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VADC18_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_rgs_qi_vaud22_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_OCSTATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VAUD22_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VAUD22_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_rgs_qi_vaud28_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_OCSTATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VAUD28_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VAUD28_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_rgs_qi_vcama_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_OCSTATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VCAMA_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_RGS_QI_VCAMA_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_ad_spk_oc_det_d_l(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_OCSTATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_AD_SPK_OC_DET_D_L_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_AD_SPK_OC_DET_D_L_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_top_ad_spk_oc_det_ab_l(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_OCSTATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_TOP_AD_SPK_OC_DET_AB_L_MASK),
                           (kal_uint32)(MT6392_PMIC_TOP_AD_SPK_OC_DET_AB_L_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_vxo22_pg_deb(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_PGSTATUS),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_VXO22_PG_DEB_MASK),
                           (kal_uint32)(MT6392_PMIC_VXO22_PG_DEB_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_vmch_pg_deb(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_PGSTATUS),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_VMCH_PG_DEB_MASK),
                           (kal_uint32)(MT6392_PMIC_VMCH_PG_DEB_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_vmc_pg_deb(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_PGSTATUS),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_VMC_PG_DEB_MASK),
                           (kal_uint32)(MT6392_PMIC_VMC_PG_DEB_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_vemc3v3_pg_deb(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_PGSTATUS),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_VEMC3V3_PG_DEB_MASK),
                           (kal_uint32)(MT6392_PMIC_VEMC3V3_PG_DEB_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_vusb_pg_deb(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_PGSTATUS),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_VUSB_PG_DEB_MASK),
                           (kal_uint32)(MT6392_PMIC_VUSB_PG_DEB_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_vm_pg_deb(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_PGSTATUS),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_VM_PG_DEB_MASK),
                           (kal_uint32)(MT6392_PMIC_VM_PG_DEB_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_vm25_pg_deb(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_PGSTATUS),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_VM25_PG_DEB_MASK),
                           (kal_uint32)(MT6392_PMIC_VM25_PG_DEB_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_vgp2_pg_deb(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_PGSTATUS),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_VGP2_PG_DEB_MASK),
                           (kal_uint32)(MT6392_PMIC_VGP2_PG_DEB_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_vio28_pg_deb(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_PGSTATUS),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_VIO28_PG_DEB_MASK),
                           (kal_uint32)(MT6392_PMIC_VIO28_PG_DEB_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_vio18_pg_deb(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_PGSTATUS),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_VIO18_PG_DEB_MASK),
                           (kal_uint32)(MT6392_PMIC_VIO18_PG_DEB_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_vsys_pg_deb(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_PGSTATUS),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_VSYS_PG_DEB_MASK),
                           (kal_uint32)(MT6392_PMIC_VSYS_PG_DEB_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_vproc_pg_deb(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_PGSTATUS),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_VPROC_PG_DEB_MASK),
                           (kal_uint32)(MT6392_PMIC_VPROC_PG_DEB_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_vcore_pg_deb(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_PGSTATUS),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_VCORE_PG_DEB_MASK),
                           (kal_uint32)(MT6392_PMIC_VCORE_PG_DEB_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_vadc18_pg_deb(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_PGSTATUS),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_VADC18_PG_DEB_MASK),
                           (kal_uint32)(MT6392_PMIC_VADC18_PG_DEB_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_pmu_test_mode_scan(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_CHRSTATUS),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_PMU_TEST_MODE_SCAN_MASK),
                           (kal_uint32)(MT6392_PMIC_PMU_TEST_MODE_SCAN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_pwrkey_deb(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_CHRSTATUS),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_PWRKEY_DEB_MASK),
                           (kal_uint32)(MT6392_PMIC_PWRKEY_DEB_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_fchrkey_deb(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_CHRSTATUS),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_FCHRKEY_DEB_MASK),
                           (kal_uint32)(MT6392_PMIC_FCHRKEY_DEB_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_vbat_ov(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_CHRSTATUS),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_VBAT_OV_MASK),
                           (kal_uint32)(MT6392_PMIC_VBAT_OV_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_pchr_chrdet(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_CHRSTATUS),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_PCHR_CHRDET_MASK),
                           (kal_uint32)(MT6392_PMIC_PCHR_CHRDET_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_ro_baton_undet(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_CHRSTATUS),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RO_BATON_UNDET_MASK),
                           (kal_uint32)(MT6392_PMIC_RO_BATON_UNDET_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rtc_xtal_det_done(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_CHRSTATUS),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RTC_XTAL_DET_DONE_MASK),
                           (kal_uint32)(MT6392_PMIC_RTC_XTAL_DET_DONE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_xosc32_enb_det(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_CHRSTATUS),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_XOSC32_ENB_DET_MASK),
                           (kal_uint32)(MT6392_PMIC_XOSC32_ENB_DET_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rtc_xtal_det_rsv(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CHRSTATUS),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RTC_XTAL_DET_RSV_MASK),
                             (kal_uint32)(MT6392_PMIC_RTC_XTAL_DET_RSV_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spi_tdsel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TDSEL_CON),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPI_TDSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPI_TDSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_pmu_tdsel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TDSEL_CON),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_PMU_TDSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_PMU_TDSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_spi_rdsel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_RDSEL_CON),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPI_RDSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPI_RDSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_pmu_rdsel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_RDSEL_CON),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_PMU_RDSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_PMU_RDSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_smt_sysrstb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SMT_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SMT_SYSRSTB_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SMT_SYSRSTB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_smt_int(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SMT_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SMT_INT_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SMT_INT_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_smt_srclken(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SMT_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SMT_SRCLKEN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SMT_SRCLKEN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_smt_rtc_32k1v8(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SMT_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SMT_RTC_32K1V8_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SMT_RTC_32K1V8_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_smt_spi_clk(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SMT_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SMT_SPI_CLK_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SMT_SPI_CLK_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_smt_spi_csn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SMT_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SMT_SPI_CSN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SMT_SPI_CSN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_smt_spi_mosi(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SMT_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SMT_SPI_MOSI_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SMT_SPI_MOSI_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_smt_spi_miso(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_SMT_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SMT_SPI_MISO_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SMT_SPI_MISO_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_octl_int(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DRV_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_OCTL_INT_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_OCTL_INT_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_octl_srclken(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DRV_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_OCTL_SRCLKEN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_OCTL_SRCLKEN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_octl_rtc_32k1v8(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DRV_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_OCTL_RTC_32K1V8_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_OCTL_RTC_32K1V8_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_octl_spi_clk(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DRV_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_OCTL_SPI_CLK_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_OCTL_SPI_CLK_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_octl_spi_csn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DRV_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_OCTL_SPI_CSN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_OCTL_SPI_CSN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_octl_spi_mosi(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DRV_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_OCTL_SPI_MOSI_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_OCTL_SPI_MOSI_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_octl_spi_miso(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DRV_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_OCTL_SPI_MISO_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_OCTL_SPI_MISO_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_int_en_spkl_ab(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_INT_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_SPKL_AB_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_SPKL_AB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_int_en_spkl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_INT_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_SPKL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_SPKL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_int_en_bat_l(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_INT_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_BAT_L_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_BAT_L_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_int_en_bat_h(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_INT_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_BAT_H_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_BAT_H_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_int_en_watchdog(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_INT_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_WATCHDOG_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_WATCHDOG_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_int_en_pwrkey(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_INT_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_PWRKEY_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_PWRKEY_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_int_en_thr_l(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_INT_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_THR_L_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_THR_L_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_int_en_thr_h(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_INT_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_THR_H_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_THR_H_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_int_en_vbaton_undet(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_INT_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_VBATON_UNDET_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_VBATON_UNDET_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_int_en_bvalid_det(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_INT_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_BVALID_DET_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_BVALID_DET_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_int_en_chrdet(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_INT_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_CHRDET_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_CHRDET_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_int_en_ov(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_INT_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_OV_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_OV_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_int_en_ldo(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_INT_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_LDO_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_LDO_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_int_en_fchrkey(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_INT_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_FCHRKEY_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_FCHRKEY_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_int_en_release_pwrkey(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_INT_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_RELEASE_PWRKEY_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_RELEASE_PWRKEY_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_int_en_release_fchrkey(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_INT_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_RELEASE_FCHRKEY_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_RELEASE_FCHRKEY_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_int_en_rtc(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_INT_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_RTC_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_RTC_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_int_en_vproc(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_INT_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_VPROC_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_VPROC_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_int_en_vsys(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_INT_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_VSYS_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_VSYS_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_int_en_vcore(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_INT_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_VCORE_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_VCORE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_int_en_type_c_cc(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_INT_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_TYPE_C_CC_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_TYPE_C_CC_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_int_en_typec_h_max(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_INT_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_TYPEC_H_MAX_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_TYPEC_H_MAX_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_int_en_typec_h_min(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_INT_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_TYPEC_H_MIN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_TYPEC_H_MIN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_int_en_typec_l_max(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_INT_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_TYPEC_L_MAX_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_TYPEC_L_MAX_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_int_en_typec_l_min(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_INT_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_TYPEC_L_MIN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_TYPEC_L_MIN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_int_en_thr_max(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_INT_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_THR_MAX_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_THR_MAX_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_int_en_thr_min(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_INT_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_THR_MIN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_THR_MIN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_int_en_nag_c_dltv(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_INT_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_NAG_C_DLTV_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_INT_EN_NAG_C_DLTV_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_polarity(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_INT_MISC_CON),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_POLARITY_MASK),
                             (kal_uint32)(MT6392_PMIC_POLARITY_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_polarity_vbaton_undet(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_INT_MISC_CON),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_POLARITY_VBATON_UNDET_MASK),
                             (kal_uint32)(MT6392_PMIC_POLARITY_VBATON_UNDET_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_polarity_bvalid_det(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_INT_MISC_CON),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_POLARITY_BVALID_DET_MASK),
                             (kal_uint32)(MT6392_PMIC_POLARITY_BVALID_DET_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_fchrkey_int_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_INT_MISC_CON),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_FCHRKEY_INT_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_FCHRKEY_INT_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_pwrkey_int_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_INT_MISC_CON),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_PWRKEY_INT_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_PWRKEY_INT_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_ivgen_ext_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_INT_MISC_CON),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_IVGEN_EXT_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_IVGEN_EXT_EN_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_rg_int_status_spkl_ab(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_INT_STATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_SPKL_AB_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_SPKL_AB_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_int_status_spkl(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_INT_STATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_SPKL_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_SPKL_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_int_status_bat_l(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_INT_STATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_BAT_L_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_BAT_L_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_int_status_bat_h(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_INT_STATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_BAT_H_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_BAT_H_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_int_status_watchdog(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_INT_STATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_WATCHDOG_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_WATCHDOG_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_int_status_pwrkey(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_INT_STATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_PWRKEY_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_PWRKEY_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_int_status_thr_l(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_INT_STATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_THR_L_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_THR_L_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_int_status_thr_h(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_INT_STATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_THR_H_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_THR_H_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_int_status_vbaton_undet(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_INT_STATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_VBATON_UNDET_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_VBATON_UNDET_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_int_status_bvalid_det(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_INT_STATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_BVALID_DET_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_BVALID_DET_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_int_status_chrdet(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_INT_STATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_CHRDET_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_CHRDET_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_int_status_ov(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_INT_STATUS0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_OV_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_OV_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_int_status_ldo(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_INT_STATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_LDO_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_LDO_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_int_status_fchrkey(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_INT_STATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_FCHRKEY_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_FCHRKEY_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_int_status_release_pwrkey(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_INT_STATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_RELEASE_PWRKEY_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_RELEASE_PWRKEY_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_int_status_release_fchrkey(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_INT_STATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_RELEASE_FCHRKEY_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_RELEASE_FCHRKEY_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_int_status_rtc(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_INT_STATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_RTC_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_RTC_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_int_status_vproc(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_INT_STATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_VPROC_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_VPROC_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_int_status_vsys(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_INT_STATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_VSYS_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_VSYS_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_int_status_vcore(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_INT_STATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_VCORE_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_VCORE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_int_status_type_c_cc(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_INT_STATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_TYPE_C_CC_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_TYPE_C_CC_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_int_status_typec_h_max(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_INT_STATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_TYPEC_H_MAX_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_TYPEC_H_MAX_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_int_status_typec_h_min(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_INT_STATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_TYPEC_H_MIN_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_TYPEC_H_MIN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_int_status_typec_l_max(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_INT_STATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_TYPEC_L_MAX_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_TYPEC_L_MAX_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_int_status_typec_l_min(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_INT_STATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_TYPEC_L_MIN_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_TYPEC_L_MIN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_int_status_thr_max(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_INT_STATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_THR_MAX_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_THR_MAX_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_int_status_thr_min(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_INT_STATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_THR_MIN_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_THR_MIN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_int_status_nag_c_dltv(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_INT_STATUS1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_NAG_C_DLTV_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_INT_STATUS_NAG_C_DLTV_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_oc_gear_bvalid_det(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_OC_GEAR_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_OC_GEAR_BVALID_DET_MASK),
                             (kal_uint32)(MT6392_PMIC_OC_GEAR_BVALID_DET_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_oc_gear_vbaton_undet(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_OC_GEAR_1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_OC_GEAR_VBATON_UNDET_MASK),
                             (kal_uint32)(MT6392_PMIC_OC_GEAR_VBATON_UNDET_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_oc_gear_ldo(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_OC_GEAR_2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_OC_GEAR_LDO_MASK),
                             (kal_uint32)(MT6392_PMIC_OC_GEAR_LDO_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vproc_oc_thd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_OC_CTL_VPROC),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VPROC_OC_THD_MASK),
                             (kal_uint32)(MT6392_PMIC_VPROC_OC_THD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vproc_oc_wnd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_OC_CTL_VPROC),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VPROC_OC_WND_MASK),
                             (kal_uint32)(MT6392_PMIC_VPROC_OC_WND_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vproc_deg_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_OC_CTL_VPROC),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VPROC_DEG_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_VPROC_DEG_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vsys_oc_thd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_OC_CTL_VSYS),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSYS_OC_THD_MASK),
                             (kal_uint32)(MT6392_PMIC_VSYS_OC_THD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vsys_oc_wnd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_OC_CTL_VSYS),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSYS_OC_WND_MASK),
                             (kal_uint32)(MT6392_PMIC_VSYS_OC_WND_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vsys_deg_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_OC_CTL_VSYS),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSYS_DEG_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_VSYS_DEG_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcore_oc_thd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_OC_CTL_VCORE),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCORE_OC_THD_MASK),
                             (kal_uint32)(MT6392_PMIC_VCORE_OC_THD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcore_oc_wnd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_OC_CTL_VCORE),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCORE_OC_WND_MASK),
                             (kal_uint32)(MT6392_PMIC_VCORE_OC_WND_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcore_deg_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_OC_CTL_VCORE),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCORE_DEG_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_VCORE_DEG_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_fqmtr_tcksel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_FQMTR_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_FQMTR_TCKSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_FQMTR_TCKSEL_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_fqmtr_busy(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_FQMTR_CON0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_FQMTR_BUSY_MASK),
                           (kal_uint32)(MT6392_PMIC_FQMTR_BUSY_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_fqmtr_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_FQMTR_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_FQMTR_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_FQMTR_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_fqmtr_winset(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_FQMTR_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_FQMTR_WINSET_MASK),
                             (kal_uint32)(MT6392_PMIC_FQMTR_WINSET_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_fqmtr_data(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_FQMTR_CON2),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_FQMTR_DATA_MASK),
                           (kal_uint32)(MT6392_PMIC_FQMTR_DATA_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_spi_con(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_RG_SPI_CON),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SPI_CON_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SPI_CON_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_dew_dio_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DEW_DIO_EN),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_DEW_DIO_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_DEW_DIO_EN_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_dew_read_test(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DEW_READ_TEST),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DEW_READ_TEST_MASK),
                           (kal_uint32)(MT6392_PMIC_DEW_READ_TEST_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_dew_write_test(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DEW_WRITE_TEST),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_DEW_WRITE_TEST_MASK),
                             (kal_uint32)(MT6392_PMIC_DEW_WRITE_TEST_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_dew_crc_swrst(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DEW_CRC_SWRST),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_DEW_CRC_SWRST_MASK),
                             (kal_uint32)(MT6392_PMIC_DEW_CRC_SWRST_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_dew_crc_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DEW_CRC_EN),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_DEW_CRC_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_DEW_CRC_EN_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_dew_crc_val(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DEW_CRC_VAL),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DEW_CRC_VAL_MASK),
                           (kal_uint32)(MT6392_PMIC_DEW_CRC_VAL_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_dew_dbg_mon_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DEW_DBG_MON_SEL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_DEW_DBG_MON_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_DEW_DBG_MON_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_dew_cipher_key_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DEW_CIPHER_KEY_SEL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_DEW_CIPHER_KEY_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_DEW_CIPHER_KEY_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_dew_cipher_iv_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DEW_CIPHER_IV_SEL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_DEW_CIPHER_IV_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_DEW_CIPHER_IV_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_dew_cipher_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DEW_CIPHER_EN),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_DEW_CIPHER_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_DEW_CIPHER_EN_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_dew_cipher_rdy(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DEW_CIPHER_RDY),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DEW_CIPHER_RDY_MASK),
                           (kal_uint32)(MT6392_PMIC_DEW_CIPHER_RDY_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_dew_cipher_mode(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DEW_CIPHER_MODE),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_DEW_CIPHER_MODE_MASK),
                             (kal_uint32)(MT6392_PMIC_DEW_CIPHER_MODE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_dew_cipher_swrst(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DEW_CIPHER_SWRST),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_DEW_CIPHER_SWRST_MASK),
                             (kal_uint32)(MT6392_PMIC_DEW_CIPHER_SWRST_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_dew_rddmy_no(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DEW_RDDMY_NO),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_DEW_RDDMY_NO_MASK),
                             (kal_uint32)(MT6392_PMIC_DEW_RDDMY_NO_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_dew_rdata_dly_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DEW_RDATA_DLY_SEL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_DEW_RDATA_DLY_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_DEW_RDATA_DLY_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_osc_75k_trim(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CLK_TRIM_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_OSC_75K_TRIM_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_OSC_75K_TRIM_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_osc_75k_trim_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CLK_TRIM_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_OSC_75K_TRIM_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_OSC_75K_TRIM_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_osc_75k_trim_rate(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_CLK_TRIM_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_OSC_75K_TRIM_RATE_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_OSC_75K_TRIM_RATE_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_rg_osc_75k_trim_out(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_CLK_TRIM_CON0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_OSC_75K_TRIM_OUT_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_OSC_75K_TRIM_OUT_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_smps_testmode_b(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SMPS_TESTMODE_B_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SMPS_TESTMODE_B_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_qi_vproc_dig_mon(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_BUCK_CON1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_QI_VPROC_DIG_MON_MASK),
                           (kal_uint32)(MT6392_PMIC_QI_VPROC_DIG_MON_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_qi_vsys_dig_mon(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_BUCK_CON1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_QI_VSYS_DIG_MON_MASK),
                           (kal_uint32)(MT6392_PMIC_QI_VSYS_DIG_MON_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_qi_vcore_dig_mon(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_BUCK_CON1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_QI_VCORE_DIG_MON_MASK),
                           (kal_uint32)(MT6392_PMIC_QI_VCORE_DIG_MON_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_vsleep_src0(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSLEEP_SRC0_MASK),
                             (kal_uint32)(MT6392_PMIC_VSLEEP_SRC0_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vsleep_src1(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSLEEP_SRC1_MASK),
                             (kal_uint32)(MT6392_PMIC_VSLEEP_SRC1_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_r2r_src0(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_CON3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_R2R_SRC0_MASK),
                             (kal_uint32)(MT6392_PMIC_R2R_SRC0_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_r2r_src1(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_CON3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_R2R_SRC1_MASK),
                             (kal_uint32)(MT6392_PMIC_R2R_SRC1_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_buck_osc_sel_src0(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_CON4),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_BUCK_OSC_SEL_SRC0_MASK),
                             (kal_uint32)(MT6392_PMIC_BUCK_OSC_SEL_SRC0_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_srclken_dly_src1(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_CON4),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_SRCLKEN_DLY_SRC1_MASK),
                             (kal_uint32)(MT6392_PMIC_SRCLKEN_DLY_SRC1_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_buck_con5_rsv0(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_CON5),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_BUCK_CON5_RSV0_MASK),
                             (kal_uint32)(MT6392_PMIC_BUCK_CON5_RSV0_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vproc_triml(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VPROC_TRIML_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VPROC_TRIML_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vproc_trimh(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VPROC_TRIMH_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VPROC_TRIMH_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vproc_csm(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VPROC_CSM_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VPROC_CSM_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vproc_zxos_trim(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VPROC_ZXOS_TRIM_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VPROC_ZXOS_TRIM_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vproc_rzsel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VPROC_RZSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VPROC_RZSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vproc_cc(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VPROC_CC_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VPROC_CC_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vproc_csr(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VPROC_CSR_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VPROC_CSR_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vproc_csl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VPROC_CSL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VPROC_CSL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vproc_zx_os(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VPROC_ZX_OS_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VPROC_ZX_OS_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vproc_avp_os(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VPROC_AVP_OS_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VPROC_AVP_OS_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vproc_avp_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VPROC_AVP_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VPROC_AVP_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vproc_modeset(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VPROC_MODESET_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VPROC_MODESET_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vproc_ndis_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VPROC_NDIS_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VPROC_NDIS_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vproc_slp(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VPROC_SLP_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VPROC_SLP_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_qi_vproc_vsleep(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_QI_VPROC_VSLEEP_MASK),
                             (kal_uint32)(MT6392_PMIC_QI_VPROC_VSLEEP_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vproc_rsv(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON4),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VPROC_RSV_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VPROC_RSV_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vproc_en_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON5),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VPROC_EN_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_VPROC_EN_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vproc_vosel_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON5),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VPROC_VOSEL_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_VPROC_VOSEL_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vproc_dlc_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON5),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VPROC_DLC_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_VPROC_DLC_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vproc_burst_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON5),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VPROC_BURST_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_VPROC_BURST_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vproc_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VPROC_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_VPROC_EN_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_qi_vproc_stb(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_VPROC_CON7),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_QI_VPROC_STB_MASK),
                           (kal_uint32)(MT6392_PMIC_QI_VPROC_STB_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_qi_vproc_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_VPROC_CON7),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_QI_VPROC_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_QI_VPROC_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_qi_vproc_oc_status(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_VPROC_CON7),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_QI_VPROC_OC_STATUS_MASK),
                           (kal_uint32)(MT6392_PMIC_QI_VPROC_OC_STATUS_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_vproc_sfchg_frate(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VPROC_SFCHG_FRATE_MASK),
                             (kal_uint32)(MT6392_PMIC_VPROC_SFCHG_FRATE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vproc_sfchg_fen(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VPROC_SFCHG_FEN_MASK),
                             (kal_uint32)(MT6392_PMIC_VPROC_SFCHG_FEN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vproc_sfchg_rrate(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VPROC_SFCHG_RRATE_MASK),
                             (kal_uint32)(MT6392_PMIC_VPROC_SFCHG_RRATE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vproc_sfchg_ren(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VPROC_SFCHG_REN_MASK),
                             (kal_uint32)(MT6392_PMIC_VPROC_SFCHG_REN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vproc_vosel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON9),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VPROC_VOSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_VPROC_VOSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vproc_vosel_on(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VPROC_VOSEL_ON_MASK),
                             (kal_uint32)(MT6392_PMIC_VPROC_VOSEL_ON_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vproc_vosel_sleep(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON11),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VPROC_VOSEL_SLEEP_MASK),
                             (kal_uint32)(MT6392_PMIC_VPROC_VOSEL_SLEEP_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_ni_vproc_vosel(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_VPROC_CON12),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_NI_VPROC_VOSEL_MASK),
                           (kal_uint32)(MT6392_PMIC_NI_VPROC_VOSEL_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_vproc_burst(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON13),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VPROC_BURST_MASK),
                             (kal_uint32)(MT6392_PMIC_VPROC_BURST_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vproc_burst_on(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON13),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VPROC_BURST_ON_MASK),
                             (kal_uint32)(MT6392_PMIC_VPROC_BURST_ON_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vproc_burst_sleep(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON13),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VPROC_BURST_SLEEP_MASK),
                             (kal_uint32)(MT6392_PMIC_VPROC_BURST_SLEEP_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_qi_vproc_burst(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_VPROC_CON13),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_QI_VPROC_BURST_MASK),
                           (kal_uint32)(MT6392_PMIC_QI_VPROC_BURST_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_vproc_dlc(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON14),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VPROC_DLC_MASK),
                             (kal_uint32)(MT6392_PMIC_VPROC_DLC_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vproc_dlc_on(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON14),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VPROC_DLC_ON_MASK),
                             (kal_uint32)(MT6392_PMIC_VPROC_DLC_ON_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vproc_dlc_sleep(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON14),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VPROC_DLC_SLEEP_MASK),
                             (kal_uint32)(MT6392_PMIC_VPROC_DLC_SLEEP_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_qi_vproc_dlc(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_VPROC_CON14),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_QI_VPROC_DLC_MASK),
                           (kal_uint32)(MT6392_PMIC_QI_VPROC_DLC_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_vproc_dlc_n(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON15),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VPROC_DLC_N_MASK),
                             (kal_uint32)(MT6392_PMIC_VPROC_DLC_N_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vproc_dlc_n_on(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON15),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VPROC_DLC_N_ON_MASK),
                             (kal_uint32)(MT6392_PMIC_VPROC_DLC_N_ON_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vproc_dlc_n_sleep(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON15),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VPROC_DLC_N_SLEEP_MASK),
                             (kal_uint32)(MT6392_PMIC_VPROC_DLC_N_SLEEP_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_qi_vproc_dlc_n(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_VPROC_CON15),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_QI_VPROC_DLC_N_MASK),
                           (kal_uint32)(MT6392_PMIC_QI_VPROC_DLC_N_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_vproc_transtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON18),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VPROC_TRANSTD_MASK),
                             (kal_uint32)(MT6392_PMIC_VPROC_TRANSTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vproc_vosel_trans_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON18),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VPROC_VOSEL_TRANS_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_VPROC_VOSEL_TRANS_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vproc_vosel_trans_once(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON18),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VPROC_VOSEL_TRANS_ONCE_MASK),
                             (kal_uint32)(MT6392_PMIC_VPROC_VOSEL_TRANS_ONCE_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_ni_vproc_vosel_trans(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_VPROC_CON18),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_NI_VPROC_VOSEL_TRANS_MASK),
                           (kal_uint32)(MT6392_PMIC_NI_VPROC_VOSEL_TRANS_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_vproc_vsleep_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON18),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VPROC_VSLEEP_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_VPROC_VSLEEP_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vproc_r2r_pdn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON18),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VPROC_R2R_PDN_MASK),
                             (kal_uint32)(MT6392_PMIC_VPROC_R2R_PDN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vproc_vsleep_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VPROC_CON18),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VPROC_VSLEEP_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_VPROC_VSLEEP_SEL_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_ni_vproc_r2r_pdn(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_VPROC_CON18),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_NI_VPROC_R2R_PDN_MASK),
                           (kal_uint32)(MT6392_PMIC_NI_VPROC_R2R_PDN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_ni_vproc_vsleep_sel(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_VPROC_CON18),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_NI_VPROC_VSLEEP_SEL_MASK),
                           (kal_uint32)(MT6392_PMIC_NI_VPROC_VSLEEP_SEL_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_vsys_triml(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VSYS_TRIML_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VSYS_TRIML_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vsys_trimh(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VSYS_TRIMH_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VSYS_TRIMH_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vsys_csm(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VSYS_CSM_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VSYS_CSM_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vsys_zxos_trim(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VSYS_ZXOS_TRIM_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VSYS_ZXOS_TRIM_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vsys_rzsel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VSYS_RZSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VSYS_RZSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vsys_cc(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VSYS_CC_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VSYS_CC_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vsys_csr(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VSYS_CSR_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VSYS_CSR_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vsys_csl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VSYS_CSL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VSYS_CSL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vsys_zx_os(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VSYS_ZX_OS_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VSYS_ZX_OS_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vsys_avp_os(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VSYS_AVP_OS_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VSYS_AVP_OS_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vsys_avp_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VSYS_AVP_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VSYS_AVP_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vsys_modeset(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VSYS_MODESET_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VSYS_MODESET_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vsys_ndis_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VSYS_NDIS_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VSYS_NDIS_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vsys_slp(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VSYS_SLP_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VSYS_SLP_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_qi_vsys_vsleep(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_QI_VSYS_VSLEEP_MASK),
                             (kal_uint32)(MT6392_PMIC_QI_VSYS_VSLEEP_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vsys_rsv(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON4),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VSYS_RSV_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VSYS_RSV_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vsys_en_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON5),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSYS_EN_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_VSYS_EN_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vsys_vosel_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON5),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSYS_VOSEL_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_VSYS_VOSEL_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vsys_dlc_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON5),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSYS_DLC_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_VSYS_DLC_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vsys_burst_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON5),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSYS_BURST_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_VSYS_BURST_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vsys_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSYS_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_VSYS_EN_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_qi_vsys_stb(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_VSYS_CON7),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_QI_VSYS_STB_MASK),
                           (kal_uint32)(MT6392_PMIC_QI_VSYS_STB_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_qi_vsys_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_VSYS_CON7),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_QI_VSYS_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_QI_VSYS_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_qi_vsys_oc_status(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_VSYS_CON7),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_QI_VSYS_OC_STATUS_MASK),
                           (kal_uint32)(MT6392_PMIC_QI_VSYS_OC_STATUS_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_vsys_sfchg_frate(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSYS_SFCHG_FRATE_MASK),
                             (kal_uint32)(MT6392_PMIC_VSYS_SFCHG_FRATE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vsys_sfchg_fen(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSYS_SFCHG_FEN_MASK),
                             (kal_uint32)(MT6392_PMIC_VSYS_SFCHG_FEN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vsys_sfchg_rrate(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSYS_SFCHG_RRATE_MASK),
                             (kal_uint32)(MT6392_PMIC_VSYS_SFCHG_RRATE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vsys_sfchg_ren(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSYS_SFCHG_REN_MASK),
                             (kal_uint32)(MT6392_PMIC_VSYS_SFCHG_REN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vsys_vosel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON9),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSYS_VOSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_VSYS_VOSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vsys_vosel_on(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSYS_VOSEL_ON_MASK),
                             (kal_uint32)(MT6392_PMIC_VSYS_VOSEL_ON_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vsys_vosel_sleep(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON11),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSYS_VOSEL_SLEEP_MASK),
                             (kal_uint32)(MT6392_PMIC_VSYS_VOSEL_SLEEP_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_ni_vsys_vosel(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_VSYS_CON12),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_NI_VSYS_VOSEL_MASK),
                           (kal_uint32)(MT6392_PMIC_NI_VSYS_VOSEL_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_vsys_burst(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON13),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSYS_BURST_MASK),
                             (kal_uint32)(MT6392_PMIC_VSYS_BURST_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vsys_burst_on(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON13),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSYS_BURST_ON_MASK),
                             (kal_uint32)(MT6392_PMIC_VSYS_BURST_ON_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vsys_burst_sleep(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON13),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSYS_BURST_SLEEP_MASK),
                             (kal_uint32)(MT6392_PMIC_VSYS_BURST_SLEEP_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_qi_vsys_burst(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_VSYS_CON13),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_QI_VSYS_BURST_MASK),
                           (kal_uint32)(MT6392_PMIC_QI_VSYS_BURST_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_vsys_dlc(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON14),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSYS_DLC_MASK),
                             (kal_uint32)(MT6392_PMIC_VSYS_DLC_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vsys_dlc_on(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON14),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSYS_DLC_ON_MASK),
                             (kal_uint32)(MT6392_PMIC_VSYS_DLC_ON_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vsys_dlc_sleep(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON14),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSYS_DLC_SLEEP_MASK),
                             (kal_uint32)(MT6392_PMIC_VSYS_DLC_SLEEP_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_qi_vsys_dlc(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_VSYS_CON14),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_QI_VSYS_DLC_MASK),
                           (kal_uint32)(MT6392_PMIC_QI_VSYS_DLC_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_vsys_dlc_n(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON15),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSYS_DLC_N_MASK),
                             (kal_uint32)(MT6392_PMIC_VSYS_DLC_N_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vsys_dlc_n_on(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON15),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSYS_DLC_N_ON_MASK),
                             (kal_uint32)(MT6392_PMIC_VSYS_DLC_N_ON_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vsys_dlc_n_sleep(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON15),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSYS_DLC_N_SLEEP_MASK),
                             (kal_uint32)(MT6392_PMIC_VSYS_DLC_N_SLEEP_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_qi_vsys_dlc_n(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_VSYS_CON15),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_QI_VSYS_DLC_N_MASK),
                           (kal_uint32)(MT6392_PMIC_QI_VSYS_DLC_N_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_vsys_transtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON18),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSYS_TRANSTD_MASK),
                             (kal_uint32)(MT6392_PMIC_VSYS_TRANSTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vsys_vosel_trans_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON18),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSYS_VOSEL_TRANS_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_VSYS_VOSEL_TRANS_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vsys_vosel_trans_once(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON18),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSYS_VOSEL_TRANS_ONCE_MASK),
                             (kal_uint32)(MT6392_PMIC_VSYS_VOSEL_TRANS_ONCE_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_ni_vsys_vosel_trans(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_VSYS_CON18),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_NI_VSYS_VOSEL_TRANS_MASK),
                           (kal_uint32)(MT6392_PMIC_NI_VSYS_VOSEL_TRANS_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_vsys_vsleep_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON18),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSYS_VSLEEP_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_VSYS_VSLEEP_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vsys_r2r_pdn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON18),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSYS_R2R_PDN_MASK),
                             (kal_uint32)(MT6392_PMIC_VSYS_R2R_PDN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vsys_vsleep_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VSYS_CON18),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VSYS_VSLEEP_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_VSYS_VSLEEP_SEL_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_ni_vsys_r2r_pdn(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_VSYS_CON18),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_NI_VSYS_R2R_PDN_MASK),
                           (kal_uint32)(MT6392_PMIC_NI_VSYS_R2R_PDN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_ni_vsys_vsleep_sel(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_VSYS_CON18),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_NI_VSYS_VSLEEP_SEL_MASK),
                           (kal_uint32)(MT6392_PMIC_NI_VSYS_VSLEEP_SEL_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_buck_vsys_oc_status(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_BUCK_OC_CON0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_BUCK_VSYS_OC_STATUS_MASK),
                           (kal_uint32)(MT6392_PMIC_BUCK_VSYS_OC_STATUS_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_buck_vcore_oc_status(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_BUCK_OC_CON0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_BUCK_VCORE_OC_STATUS_MASK),
                           (kal_uint32)(MT6392_PMIC_BUCK_VCORE_OC_STATUS_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_buck_vproc_oc_status(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_BUCK_OC_CON0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_BUCK_VPROC_OC_STATUS_MASK),
                           (kal_uint32)(MT6392_PMIC_BUCK_VPROC_OC_STATUS_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_buck_vsys_oc_int_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_OC_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_BUCK_VSYS_OC_INT_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_BUCK_VSYS_OC_INT_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_buck_vcore_oc_int_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_OC_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_BUCK_VCORE_OC_INT_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_BUCK_VCORE_OC_INT_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_buck_vproc_oc_int_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_OC_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_BUCK_VPROC_OC_INT_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_BUCK_VPROC_OC_INT_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_buck_vsys_en_oc_sdn_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_OC_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_BUCK_VSYS_EN_OC_SDN_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_BUCK_VSYS_EN_OC_SDN_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_buck_vcore_en_oc_sdn_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_OC_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_BUCK_VCORE_EN_OC_SDN_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_BUCK_VCORE_EN_OC_SDN_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_buck_vproc_en_oc_sdn_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_OC_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_BUCK_VPROC_EN_OC_SDN_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_BUCK_VPROC_EN_OC_SDN_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_buck_vsys_oc_flag_clr(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_OC_CON3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_BUCK_VSYS_OC_FLAG_CLR_MASK),
                             (kal_uint32)(MT6392_PMIC_BUCK_VSYS_OC_FLAG_CLR_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_buck_vcore_oc_flag_clr(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_OC_CON3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_BUCK_VCORE_OC_FLAG_CLR_MASK),
                             (kal_uint32)(MT6392_PMIC_BUCK_VCORE_OC_FLAG_CLR_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_buck_vproc_oc_flag_clr(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_OC_CON3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_BUCK_VPROC_OC_FLAG_CLR_MASK),
                             (kal_uint32)(MT6392_PMIC_BUCK_VPROC_OC_FLAG_CLR_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_buck_vsys_oc_flag_clr_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_OC_CON4),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_BUCK_VSYS_OC_FLAG_CLR_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_BUCK_VSYS_OC_FLAG_CLR_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_buck_vcore_oc_flag_clr_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_OC_CON4),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_BUCK_VCORE_OC_FLAG_CLR_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_BUCK_VCORE_OC_FLAG_CLR_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_buck_vproc_oc_flag_clr_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_OC_CON4),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_BUCK_VPROC_OC_FLAG_CLR_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_BUCK_VPROC_OC_FLAG_CLR_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_buck_vproc_oc_deg_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_OC_VPROC_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_BUCK_VPROC_OC_DEG_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_BUCK_VPROC_OC_DEG_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_buck_vproc_oc_wnd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_OC_VPROC_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_BUCK_VPROC_OC_WND_MASK),
                             (kal_uint32)(MT6392_PMIC_BUCK_VPROC_OC_WND_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_buck_vproc_oc_thd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_OC_VPROC_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_BUCK_VPROC_OC_THD_MASK),
                             (kal_uint32)(MT6392_PMIC_BUCK_VPROC_OC_THD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_buck_vcore_oc_deg_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_OC_VCORE_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_BUCK_VCORE_OC_DEG_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_BUCK_VCORE_OC_DEG_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_buck_vcore_oc_wnd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_OC_VCORE_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_BUCK_VCORE_OC_WND_MASK),
                             (kal_uint32)(MT6392_PMIC_BUCK_VCORE_OC_WND_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_buck_vcore_oc_thd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_OC_VCORE_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_BUCK_VCORE_OC_THD_MASK),
                             (kal_uint32)(MT6392_PMIC_BUCK_VCORE_OC_THD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_buck_vsys_oc_deg_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_OC_VSYS_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_BUCK_VSYS_OC_DEG_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_BUCK_VSYS_OC_DEG_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_buck_vsys_oc_wnd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_OC_VSYS_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_BUCK_VSYS_OC_WND_MASK),
                             (kal_uint32)(MT6392_PMIC_BUCK_VSYS_OC_WND_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_buck_vsys_oc_thd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_OC_VSYS_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_BUCK_VSYS_OC_THD_MASK),
                             (kal_uint32)(MT6392_PMIC_BUCK_VSYS_OC_THD_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_ad_qi_vsys_oc_status(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_BUCK_ANA_MON_CON0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AD_QI_VSYS_OC_STATUS_MASK),
                           (kal_uint32)(MT6392_PMIC_AD_QI_VSYS_OC_STATUS_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_ad_qi_vcore_oc_status(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_BUCK_ANA_MON_CON0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AD_QI_VCORE_OC_STATUS_MASK),
                           (kal_uint32)(MT6392_PMIC_AD_QI_VCORE_OC_STATUS_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_ad_qi_vproc_oc_status(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_BUCK_ANA_MON_CON0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AD_QI_VPROC_OC_STATUS_MASK),
                           (kal_uint32)(MT6392_PMIC_AD_QI_VPROC_OC_STATUS_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_buck_ana_status(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_BUCK_ANA_MON_CON0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_BUCK_ANA_STATUS_MASK),
                           (kal_uint32)(MT6392_PMIC_BUCK_ANA_STATUS_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_efuse_oc_en_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_EFUSE_OC_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_EFUSE_OC_EN_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_EFUSE_OC_EN_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_efuse_oc_sdn_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_EFUSE_OC_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_EFUSE_OC_SDN_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_EFUSE_OC_SDN_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcore_triml(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCORE_TRIML_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCORE_TRIML_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcore_trimh(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCORE_TRIMH_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCORE_TRIMH_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcore_csm(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCORE_CSM_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCORE_CSM_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcore_zxos_trim(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCORE_ZXOS_TRIM_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCORE_ZXOS_TRIM_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcore_rzsel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCORE_RZSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCORE_RZSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcore_cc(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCORE_CC_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCORE_CC_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcore_csr(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCORE_CSR_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCORE_CSR_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcore_csl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCORE_CSL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCORE_CSL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcore_zx_os(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCORE_ZX_OS_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCORE_ZX_OS_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcore_avp_os(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCORE_AVP_OS_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCORE_AVP_OS_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcore_avp_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCORE_AVP_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCORE_AVP_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcore_modeset(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCORE_MODESET_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCORE_MODESET_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcore_ndis_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCORE_NDIS_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCORE_NDIS_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcore_slp(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCORE_SLP_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCORE_SLP_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_qi_vcore_vsleep(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_QI_VCORE_VSLEEP_MASK),
                             (kal_uint32)(MT6392_PMIC_QI_VCORE_VSLEEP_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcore_rsv(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON4),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCORE_RSV_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCORE_RSV_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcore_en_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON5),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCORE_EN_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_VCORE_EN_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcore_vosel_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON5),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCORE_VOSEL_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_VCORE_VOSEL_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcore_dlc_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON5),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCORE_DLC_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_VCORE_DLC_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcore_burst_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON5),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCORE_BURST_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_VCORE_BURST_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcore_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCORE_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_VCORE_EN_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_qi_vcore_stb(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_VCORE_CON7),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_QI_VCORE_STB_MASK),
                           (kal_uint32)(MT6392_PMIC_QI_VCORE_STB_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_qi_vcore_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_VCORE_CON7),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_QI_VCORE_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_QI_VCORE_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_qi_vcore_oc_status(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_VCORE_CON7),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_QI_VCORE_OC_STATUS_MASK),
                           (kal_uint32)(MT6392_PMIC_QI_VCORE_OC_STATUS_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_vcore_sfchg_frate(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCORE_SFCHG_FRATE_MASK),
                             (kal_uint32)(MT6392_PMIC_VCORE_SFCHG_FRATE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcore_sfchg_fen(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCORE_SFCHG_FEN_MASK),
                             (kal_uint32)(MT6392_PMIC_VCORE_SFCHG_FEN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcore_sfchg_rrate(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCORE_SFCHG_RRATE_MASK),
                             (kal_uint32)(MT6392_PMIC_VCORE_SFCHG_RRATE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcore_sfchg_ren(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCORE_SFCHG_REN_MASK),
                             (kal_uint32)(MT6392_PMIC_VCORE_SFCHG_REN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcore_vosel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON9),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCORE_VOSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_VCORE_VOSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcore_vosel_on(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCORE_VOSEL_ON_MASK),
                             (kal_uint32)(MT6392_PMIC_VCORE_VOSEL_ON_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcore_vosel_sleep(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON11),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCORE_VOSEL_SLEEP_MASK),
                             (kal_uint32)(MT6392_PMIC_VCORE_VOSEL_SLEEP_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_ni_vcore_vosel(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_VCORE_CON12),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_NI_VCORE_VOSEL_MASK),
                           (kal_uint32)(MT6392_PMIC_NI_VCORE_VOSEL_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_vcore_burst(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON13),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCORE_BURST_MASK),
                             (kal_uint32)(MT6392_PMIC_VCORE_BURST_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcore_burst_on(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON13),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCORE_BURST_ON_MASK),
                             (kal_uint32)(MT6392_PMIC_VCORE_BURST_ON_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcore_burst_sleep(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON13),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCORE_BURST_SLEEP_MASK),
                             (kal_uint32)(MT6392_PMIC_VCORE_BURST_SLEEP_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_qi_vcore_burst(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_VCORE_CON13),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_QI_VCORE_BURST_MASK),
                           (kal_uint32)(MT6392_PMIC_QI_VCORE_BURST_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_vcore_dlc(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON14),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCORE_DLC_MASK),
                             (kal_uint32)(MT6392_PMIC_VCORE_DLC_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcore_dlc_on(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON14),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCORE_DLC_ON_MASK),
                             (kal_uint32)(MT6392_PMIC_VCORE_DLC_ON_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcore_dlc_sleep(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON14),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCORE_DLC_SLEEP_MASK),
                             (kal_uint32)(MT6392_PMIC_VCORE_DLC_SLEEP_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_qi_vcore_dlc(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_VCORE_CON14),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_QI_VCORE_DLC_MASK),
                           (kal_uint32)(MT6392_PMIC_QI_VCORE_DLC_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_vcore_dlc_n(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON15),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCORE_DLC_N_MASK),
                             (kal_uint32)(MT6392_PMIC_VCORE_DLC_N_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcore_dlc_n_on(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON15),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCORE_DLC_N_ON_MASK),
                             (kal_uint32)(MT6392_PMIC_VCORE_DLC_N_ON_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcore_dlc_n_sleep(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON15),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCORE_DLC_N_SLEEP_MASK),
                             (kal_uint32)(MT6392_PMIC_VCORE_DLC_N_SLEEP_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_qi_vcore_dlc_n(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_VCORE_CON15),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_QI_VCORE_DLC_N_MASK),
                           (kal_uint32)(MT6392_PMIC_QI_VCORE_DLC_N_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_vcore_transtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON18),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCORE_TRANSTD_MASK),
                             (kal_uint32)(MT6392_PMIC_VCORE_TRANSTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcore_vosel_trans_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON18),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCORE_VOSEL_TRANS_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_VCORE_VOSEL_TRANS_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcore_vosel_trans_once(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON18),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCORE_VOSEL_TRANS_ONCE_MASK),
                             (kal_uint32)(MT6392_PMIC_VCORE_VOSEL_TRANS_ONCE_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_ni_vcore_vosel_trans(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_VCORE_CON18),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_NI_VCORE_VOSEL_TRANS_MASK),
                           (kal_uint32)(MT6392_PMIC_NI_VCORE_VOSEL_TRANS_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_vcore_vsleep_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON18),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCORE_VSLEEP_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_VCORE_VSLEEP_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcore_r2r_pdn(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON18),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCORE_R2R_PDN_MASK),
                             (kal_uint32)(MT6392_PMIC_VCORE_R2R_PDN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcore_vsleep_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_VCORE_CON18),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCORE_VSLEEP_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_VCORE_VSLEEP_SEL_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_ni_vcore_r2r_pdn(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_VCORE_CON18),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_NI_VCORE_R2R_PDN_MASK),
                           (kal_uint32)(MT6392_PMIC_NI_VCORE_R2R_PDN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_ni_vcore_vsleep_sel(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_VCORE_CON18),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_NI_VCORE_VSLEEP_SEL_MASK),
                           (kal_uint32)(MT6392_PMIC_NI_VCORE_VSLEEP_SEL_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_k_rst_done(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_K_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_K_RST_DONE_MASK),
                             (kal_uint32)(MT6392_PMIC_K_RST_DONE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_k_map_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_K_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_K_MAP_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_K_MAP_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_k_once_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_K_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_K_ONCE_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_K_ONCE_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_k_once(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_K_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_K_ONCE_MASK),
                             (kal_uint32)(MT6392_PMIC_K_ONCE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_k_start_manual(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_K_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_K_START_MANUAL_MASK),
                             (kal_uint32)(MT6392_PMIC_K_START_MANUAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_k_src_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_K_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_K_SRC_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_K_SRC_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_k_auto_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_K_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_K_AUTO_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_K_AUTO_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_k_inv(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_K_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_K_INV_MASK),
                             (kal_uint32)(MT6392_PMIC_K_INV_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_k_control_smps(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_BUCK_K_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_K_CONTROL_SMPS_MASK),
                             (kal_uint32)(MT6392_PMIC_K_CONTROL_SMPS_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_k_result(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_BUCK_K_CON2),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_K_RESULT_MASK),
                           (kal_uint32)(MT6392_PMIC_K_RESULT_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_k_done(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_BUCK_K_CON2),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_K_DONE_MASK),
                           (kal_uint32)(MT6392_PMIC_K_DONE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_k_control(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_BUCK_K_CON2),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_K_CONTROL_MASK),
                           (kal_uint32)(MT6392_PMIC_K_CONTROL_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_qi_smps_osc_cal(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_BUCK_K_CON2),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_QI_SMPS_OSC_CAL_MASK),
                           (kal_uint32)(MT6392_PMIC_QI_SMPS_OSC_CAL_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_analdorsv1(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_ANALDORSV1_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_ANALDORSV1_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vtcxo_lp_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VTCXO_LP_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_VTCXO_LP_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vtcxo_lp_set(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VTCXO_LP_SET_MASK),
                             (kal_uint32)(MT6392_PMIC_VTCXO_LP_SET_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vtcxo_mode(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_ANALDO_CON1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VTCXO_MODE_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VTCXO_MODE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_vtcxo_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VTCXO_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VTCXO_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vtcxo_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VTCXO_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VTCXO_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vtcxo_on_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VTCXO_ON_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_VTCXO_ON_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vtcxo_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_ANALDO_CON1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VTCXO_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VTCXO_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_vaud22_lp_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VAUD22_LP_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_VAUD22_LP_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vaud22_lp_set(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VAUD22_LP_SET_MASK),
                             (kal_uint32)(MT6392_PMIC_VAUD22_LP_SET_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vaud22_mode(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_ANALDO_CON2),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VAUD22_MODE_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VAUD22_MODE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_vaud22_sense_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VAUD22_SENSE_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VAUD22_SENSE_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vaud22_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VAUD22_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VAUD22_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vaud22_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VAUD22_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VAUD22_EN_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vaud22_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_ANALDO_CON2),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VAUD22_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VAUD22_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_analdorsv2(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_ANALDORSV2_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_ANALDORSV2_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcama_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON4),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCAMA_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCAMA_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcama_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON4),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCAMA_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCAMA_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_analdorsv3(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON6),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_ANALDORSV3_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_ANALDORSV3_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vtcxo_oc_status_stbdt(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VTCXO_OC_STATUS_STBDT_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VTCXO_OC_STATUS_STBDT_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vtcxo_ndis_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VTCXO_NDIS_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VTCXO_NDIS_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vtcxo_ocfb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VTCXO_OCFB_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VTCXO_OCFB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vtcxo_cal(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VTCXO_CAL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VTCXO_CAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vaud22_ndis_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VAUD22_NDIS_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VAUD22_NDIS_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vaud22_ocfb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VAUD22_OCFB_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VAUD22_OCFB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vaud22_vosel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VAUD22_VOSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VAUD22_VOSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vaud22_cal(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VAUD22_CAL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VAUD22_CAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcama_fbsel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCAMA_FBSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCAMA_FBSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcama_ndis_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCAMA_NDIS_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCAMA_NDIS_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcama_ocfb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCAMA_OCFB_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCAMA_OCFB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcama_stb_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCAMA_STB_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCAMA_STB_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcama_vosel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCAMA_VOSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCAMA_VOSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcama_on_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCAMA_ON_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_VCAMA_ON_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcama_cal(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCAMA_CAL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCAMA_CAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_reserve_stb_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON15),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_RESERVE_STB_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_RESERVE_STB_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_aldo_reserve(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON15),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_ALDO_RESERVE_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_ALDO_RESERVE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcn35_vosel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON16),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCN35_VOSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCN35_VOSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcn35_ndis_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON16),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCN35_NDIS_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCN35_NDIS_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcn35_on_ctrl_bt(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON16),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCN35_ON_CTRL_BT_MASK),
                             (kal_uint32)(MT6392_PMIC_VCN35_ON_CTRL_BT_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcn35_ocfb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON16),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCN35_OCFB_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCN35_OCFB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcn35_en_bt(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON16),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCN35_EN_BT_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCN35_EN_BT_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcn35_cal(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON16),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCN35_CAL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCN35_CAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcn35_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON17),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCN35_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCN35_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcn35_en_wifi(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON17),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCN35_EN_WIFI_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCN35_EN_WIFI_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcn35_on_ctrl_wifi(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON17),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCN35_ON_CTRL_WIFI_MASK),
                             (kal_uint32)(MT6392_PMIC_VCN35_ON_CTRL_WIFI_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vcn35_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_ANALDO_CON17),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VCN35_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VCN35_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_vcn35_lp_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON21),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCN35_LP_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_VCN35_LP_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcn35_lp_set(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON21),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCN35_LP_SET_MASK),
                             (kal_uint32)(MT6392_PMIC_VCN35_LP_SET_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vcn35_mode(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_ANALDO_CON21),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VCN35_MODE_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VCN35_MODE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_vcn35_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON21),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCN35_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCN35_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vaud28_ndis_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON22),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VAUD28_NDIS_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VAUD28_NDIS_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vaud28_ocfb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON22),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VAUD28_OCFB_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VAUD28_OCFB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vaud28_vosel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON22),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VAUD28_VOSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VAUD28_VOSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vaud28_cal(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON22),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VAUD28_CAL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VAUD28_CAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vaud28_lp_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON23),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VAUD28_LP_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_VAUD28_LP_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vaud28_lp_set(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON23),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VAUD28_LP_SET_MASK),
                             (kal_uint32)(MT6392_PMIC_VAUD28_LP_SET_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vaud28_mode(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_ANALDO_CON23),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VAUD28_MODE_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VAUD28_MODE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_vaud28_sense_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON23),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VAUD28_SENSE_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VAUD28_SENSE_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vaud28_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON23),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VAUD28_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VAUD28_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vaud28_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON23),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VAUD28_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VAUD28_EN_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vaud28_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_ANALDO_CON23),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VAUD28_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VAUD28_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_vadc18_ndis_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON24),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VADC18_NDIS_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VADC18_NDIS_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vadc18_ocfb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON24),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VADC18_OCFB_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VADC18_OCFB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vadc18_vosel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON24),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VADC18_VOSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VADC18_VOSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vadc18_cal(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON24),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VADC18_CAL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VADC18_CAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vadc18_lp_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON25),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VADC18_LP_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_VADC18_LP_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vadc18_lp_set(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON25),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VADC18_LP_SET_MASK),
                             (kal_uint32)(MT6392_PMIC_VADC18_LP_SET_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vadc18_mode(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_ANALDO_CON25),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VADC18_MODE_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VADC18_MODE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_vadc18_sense_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON25),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VADC18_SENSE_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VADC18_SENSE_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vadc18_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON25),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VADC18_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VADC18_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vadc18_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON25),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VADC18_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VADC18_EN_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vadc18_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_ANALDO_CON25),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VADC18_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VADC18_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_vcama_oc_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON26),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCAMA_OC_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCAMA_OC_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vtcxo_oc_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON26),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VTCXO_OC_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VTCXO_OC_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vaud22_oc_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON26),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VAUD22_OC_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VAUD22_OC_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vaud28_oc_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON26),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VAUD28_OC_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VAUD28_OC_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vadc18_oc_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON26),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VADC18_OC_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VADC18_OC_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vaud22_on_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON27),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VAUD22_ON_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_VAUD22_ON_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vaud28_on_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON27),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VAUD28_ON_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_VAUD28_ON_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vadc18_on_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON27),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VADC18_ON_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_VADC18_ON_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_rgs_qi_vcama_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_ANALDO_CON28),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VCAMA_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VCAMA_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_qi_vaud28_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_ANALDO_CON28),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VAUD28_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VAUD28_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_qi_vaud22_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_ANALDO_CON28),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VAUD22_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VAUD22_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_qi_vadc18_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_ANALDO_CON28),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VADC18_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VADC18_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_qi_vtcxo_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_ANALDO_CON28),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VTCXO_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VTCXO_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_ad_da_type_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON29),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_AD_DA_TYPE_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_AD_DA_TYPE_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_ldo_type_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON29),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_LDO_TYPE_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_LDO_TYPE_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_ldo_channel_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_ANALDO_CON29),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_LDO_CHANNEL_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_LDO_CHANNEL_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vio28_lp_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VIO28_LP_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_VIO28_LP_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vio28_lp_mode_set(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VIO28_LP_MODE_SET_MASK),
                             (kal_uint32)(MT6392_PMIC_VIO28_LP_MODE_SET_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vio28_mode(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VIO28_MODE_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VIO28_MODE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_vio28_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VIO28_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VIO28_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vio28_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VIO28_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_VIO28_EN_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vio28_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VIO28_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VIO28_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_vusb_lp_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VUSB_LP_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_VUSB_LP_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vusb_lp_mode_set(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VUSB_LP_MODE_SET_MASK),
                             (kal_uint32)(MT6392_PMIC_VUSB_LP_MODE_SET_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vusb_mode(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON2),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VUSB_MODE_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VUSB_MODE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_vusb_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VUSB_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VUSB_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vusb_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VUSB_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VUSB_EN_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vusb_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON2),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VUSB_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VUSB_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_vmc_lp_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VMC_LP_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_VMC_LP_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vmc_lp_mode_set(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VMC_LP_MODE_SET_MASK),
                             (kal_uint32)(MT6392_PMIC_VMC_LP_MODE_SET_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_stb_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_STB_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_STB_SEL_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vmc_mode(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON3),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VMC_MODE_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VMC_MODE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_vmc_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VMC_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VMC_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vmc_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VMC_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VMC_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vmc_int_dis_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VMC_INT_DIS_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VMC_INT_DIS_SEL_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vmc_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON3),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VMC_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VMC_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_vmch_lp_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON5),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VMCH_LP_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_VMCH_LP_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vmch_lp_mode_set(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON5),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VMCH_LP_MODE_SET_MASK),
                             (kal_uint32)(MT6392_PMIC_VMCH_LP_MODE_SET_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vmch_mode(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON5),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VMCH_MODE_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VMCH_MODE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_vmch_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON5),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VMCH_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VMCH_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vmch_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON5),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VMCH_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VMCH_EN_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vmch_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON5),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VMCH_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VMCH_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_vemc_3v3_lp_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON6),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VEMC_3V3_LP_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_VEMC_3V3_LP_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vemc_3v3_lp_mode_set(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON6),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VEMC_3V3_LP_MODE_SET_MASK),
                             (kal_uint32)(MT6392_PMIC_VEMC_3V3_LP_MODE_SET_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vemc_3v3_mode(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON6),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VEMC_3V3_MODE_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VEMC_3V3_MODE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_vemc_3v3_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON6),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VEMC_3V3_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VEMC_3V3_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vemc_3v3_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON6),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VEMC_3V3_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VEMC_3V3_EN_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vemc_3v3_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON6),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VEMC_3V3_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VEMC_3V3_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_vgp1_lp_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VGP1_LP_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_VGP1_LP_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vgp1_lp_mode_set(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VGP1_LP_MODE_SET_MASK),
                             (kal_uint32)(MT6392_PMIC_VGP1_LP_MODE_SET_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vgp1_mode(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON7),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VGP1_MODE_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VGP1_MODE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_vgp1_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VGP1_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VGP1_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vgp1_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VGP1_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VGP1_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vgp2_lp_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VGP2_LP_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_VGP2_LP_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vgp2_lp_mode_set(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VGP2_LP_MODE_SET_MASK),
                             (kal_uint32)(MT6392_PMIC_VGP2_LP_MODE_SET_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vgp2_mode(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON8),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VGP2_MODE_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VGP2_MODE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_vgp2_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VGP2_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VGP2_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vgp2_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VGP2_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VGP2_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcn_1v8_ndis_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCN_1V8_NDIS_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCN_1V8_NDIS_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcn_1v8_on_ctrl_golden(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCN_1V8_ON_CTRL_GOLDEN_MASK),
                             (kal_uint32)(MT6392_PMIC_VCN_1V8_ON_CTRL_GOLDEN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcn_1v8_ocfb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCN_1V8_OCFB_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCN_1V8_OCFB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcn_1v8_stb_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCN_1V8_STB_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCN_1V8_STB_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcn_1v8_cal(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCN_1V8_CAL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCN_1V8_CAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcn_1v8_lp_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON11),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCN_1V8_LP_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_VCN_1V8_LP_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcn_1v8_lp_mode_set(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON11),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCN_1V8_LP_MODE_SET_MASK),
                             (kal_uint32)(MT6392_PMIC_VCN_1V8_LP_MODE_SET_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vcn_1v8_mode(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON11),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VCN_1V8_MODE_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VCN_1V8_MODE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_vcn_1v8_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON11),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCN_1V8_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCN_1V8_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcn_1v8_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON11),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCN_1V8_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCN_1V8_EN_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vcn_1v8_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON11),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VCN_1V8_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VCN_1V8_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_stb_sim1_sio(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_STB_SIM1_SIO_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_STB_SIM1_SIO_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_re_digldorsv1(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RE_DIGLDORSV1_MASK),
                             (kal_uint32)(MT6392_PMIC_RE_DIGLDORSV1_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vrtc_force_on(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON15),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VRTC_FORCE_ON_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VRTC_FORCE_ON_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vrtc_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON15),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VRTC_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_VRTC_EN_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vrtc_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON15),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VRTC_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VRTC_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_stb_sim2_sio(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON20),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_STB_SIM2_SIO_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_STB_SIM2_SIO_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_re_digldorsv2(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON20),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RE_DIGLDORSV2_MASK),
                             (kal_uint32)(MT6392_PMIC_RE_DIGLDORSV2_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vio28_ndis_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON21),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VIO28_NDIS_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VIO28_NDIS_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vio28_ocfb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON21),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VIO28_OCFB_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VIO28_OCFB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vio28_cal(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON21),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VIO28_CAL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VIO28_CAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vusb_ndis_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON23),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VUSB_NDIS_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VUSB_NDIS_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vusb_ocfb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON23),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VUSB_OCFB_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VUSB_OCFB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vusb_cal(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON23),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VUSB_CAL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VUSB_CAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vmc_ndis_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON24),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VMC_NDIS_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VMC_NDIS_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vmc_on_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON24),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VMC_ON_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_VMC_ON_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vmc_ocfb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON24),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VMC_OCFB_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VMC_OCFB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vmc_vosel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON24),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VMC_VOSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VMC_VOSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vmc_stb_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON24),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VMC_STB_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VMC_STB_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vmc_stb_sel_cal(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON24),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VMC_STB_SEL_CAL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VMC_STB_SEL_CAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vmc_cal(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON24),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VMC_CAL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VMC_CAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vmch_ndis_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON26),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VMCH_NDIS_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VMCH_NDIS_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vmch_on_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON26),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VMCH_ON_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_VMCH_ON_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vmch_ocfb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON26),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VMCH_OCFB_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VMCH_OCFB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vmch_db_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON26),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VMCH_DB_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VMCH_DB_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vmch_stb_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON26),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VMCH_STB_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VMCH_STB_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vmch_vosel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON26),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VMCH_VOSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VMCH_VOSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vmch_stb_sel_cal(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON26),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VMCH_STB_SEL_CAL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VMCH_STB_SEL_CAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vmch_cal(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON26),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VMCH_CAL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VMCH_CAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vemc_3v3_ndis_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON27),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VEMC_3V3_NDIS_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VEMC_3V3_NDIS_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vemc_3v3_on_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON27),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VEMC_3V3_ON_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_VEMC_3V3_ON_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vemc_3v3_ocfb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON27),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VEMC_3V3_OCFB_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VEMC_3V3_OCFB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vemc_3v3_dl_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON27),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VEMC_3V3_DL_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VEMC_3V3_DL_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vemc_3v3_db_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON27),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VEMC_3V3_DB_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VEMC_3V3_DB_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vemc_3v3_stb_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON27),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VEMC_3V3_STB_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VEMC_3V3_STB_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vemc_3v3_vosel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON27),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VEMC_3V3_VOSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VEMC_3V3_VOSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vemc_3v3_stb_sel_cal(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON27),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VEMC_3V3_STB_SEL_CAL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VEMC_3V3_STB_SEL_CAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vemc_3v3_cal(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON27),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VEMC_3V3_CAL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VEMC_3V3_CAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vgp1_ndis_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON28),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VGP1_NDIS_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VGP1_NDIS_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vgp1_ocfb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON28),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VGP1_OCFB_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VGP1_OCFB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vgp1_stb_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON28),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VGP1_STB_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VGP1_STB_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vgp1_vosel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON28),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VGP1_VOSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VGP1_VOSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vgp1_cal(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON28),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VGP1_CAL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VGP1_CAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vgp2_ndis_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON29),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VGP2_NDIS_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VGP2_NDIS_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vgp2_ocfb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON29),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VGP2_OCFB_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VGP2_OCFB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vgp2_stb_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON29),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VGP2_STB_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VGP2_STB_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vgp2_vosel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON29),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VGP2_VOSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VGP2_VOSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vgp2_cal(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON29),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VGP2_CAL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VGP2_CAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vgp3_ndis_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON30),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VGP3_NDIS_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VGP3_NDIS_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vgp3_ocfb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON30),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VGP3_OCFB_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VGP3_OCFB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vgp3_stb_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON30),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VGP3_STB_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VGP3_STB_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vgp3_vosel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON30),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VGP3_VOSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VGP3_VOSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vgp3_cal(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON30),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VGP3_CAL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VGP3_CAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcam_af_lp_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON31),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCAM_AF_LP_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_VCAM_AF_LP_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcam_af_lp_mode_set(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON31),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCAM_AF_LP_MODE_SET_MASK),
                             (kal_uint32)(MT6392_PMIC_VCAM_AF_LP_MODE_SET_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vcam_af_mode(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON31),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VCAM_AF_MODE_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VCAM_AF_MODE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_vcam_af_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON31),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCAM_AF_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCAM_AF_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcam_af_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON31),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCAM_AF_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCAM_AF_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcam_af_ndis_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON32),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCAM_AF_NDIS_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCAM_AF_NDIS_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcam_af_ocfb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON32),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCAM_AF_OCFB_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCAM_AF_OCFB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcam_af_on_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON32),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCAM_AF_ON_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_VCAM_AF_ON_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcam_af_stb_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON32),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCAM_AF_STB_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCAM_AF_STB_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcam_af_vosel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON32),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCAM_AF_VOSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCAM_AF_VOSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcam_af_cal(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON32),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCAM_AF_CAL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCAM_AF_CAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_re_digldorsv3(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON33),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RE_DIGLDORSV3_MASK),
                             (kal_uint32)(MT6392_PMIC_RE_DIGLDORSV3_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vsysldo_reserve(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON36),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VSYSLDO_RESERVE_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VSYSLDO_RESERVE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_digldo_rsv1(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON41),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_DIGLDO_RSV1_MASK),
                             (kal_uint32)(MT6392_PMIC_DIGLDO_RSV1_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_digldo_rsv0(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON41),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_DIGLDO_RSV0_MASK),
                             (kal_uint32)(MT6392_PMIC_DIGLDO_RSV0_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_ldo_ft(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON41),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_LDO_FT_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_LDO_FT_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vgp2_on_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON44),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VGP2_ON_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_VGP2_ON_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vgp1_on_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON44),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VGP1_ON_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_VGP1_ON_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vm_lp_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON47),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VM_LP_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_VM_LP_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vm_lp_mode_set(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON47),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VM_LP_MODE_SET_MASK),
                             (kal_uint32)(MT6392_PMIC_VM_LP_MODE_SET_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vm_mode(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON47),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VM_MODE_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VM_MODE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_vm_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON47),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VM_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VM_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vm_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON47),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VM_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VM_EN_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vm_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON47),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VM_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VM_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_vm_ndis_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON48),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VM_NDIS_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VM_NDIS_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vm_plcur_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON48),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VM_PLCUR_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VM_PLCUR_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vm_plcur_cal(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON48),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VM_PLCUR_CAL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VM_PLCUR_CAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vm_vosel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON48),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VM_VOSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VM_VOSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vm_ocfb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON48),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VM_OCFB_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VM_OCFB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vm_cal(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON48),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VM_CAL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VM_CAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vio18_lp_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON49),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VIO18_LP_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_VIO18_LP_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vio18_lp_mode_set(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON49),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VIO18_LP_MODE_SET_MASK),
                             (kal_uint32)(MT6392_PMIC_VIO18_LP_MODE_SET_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vio18_mode(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON49),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VIO18_MODE_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VIO18_MODE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_vio18_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON49),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VIO18_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VIO18_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vio18_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON49),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VIO18_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VIO18_EN_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vio18_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON49),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VIO18_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VIO18_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_vio18_ndis_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON50),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VIO18_NDIS_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VIO18_NDIS_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vio18_on_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON50),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VIO18_ON_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_VIO18_ON_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vio18_ocfb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON50),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VIO18_OCFB_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VIO18_OCFB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vio18_stb_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON50),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VIO18_STB_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VIO18_STB_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vio18_cal(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON50),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VIO18_CAL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VIO18_CAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcamd_lp_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON51),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCAMD_LP_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_VCAMD_LP_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcamd_lp_mode_set(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON51),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCAMD_LP_MODE_SET_MASK),
                             (kal_uint32)(MT6392_PMIC_VCAMD_LP_MODE_SET_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vcamd_mode(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON51),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VCAMD_MODE_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VCAMD_MODE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_vcamd_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON51),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCAMD_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCAMD_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcamd_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON51),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCAMD_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCAMD_EN_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vcamd_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON51),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VCAMD_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VCAMD_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_vcamd_ndis_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON52),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCAMD_NDIS_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCAMD_NDIS_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcamd_on_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON52),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCAMD_ON_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_VCAMD_ON_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcamd_ocfb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON52),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCAMD_OCFB_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCAMD_OCFB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcamd_stb_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON52),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCAMD_STB_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCAMD_STB_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcamd_vosel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON52),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCAMD_VOSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCAMD_VOSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcamd_cal(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON52),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCAMD_CAL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCAMD_CAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcam_io_lp_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON53),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCAM_IO_LP_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_VCAM_IO_LP_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcam_io_lp_mode_set(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON53),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCAM_IO_LP_MODE_SET_MASK),
                             (kal_uint32)(MT6392_PMIC_VCAM_IO_LP_MODE_SET_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vcam_io_mode(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON53),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VCAM_IO_MODE_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VCAM_IO_MODE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_vcam_io_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON53),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCAM_IO_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCAM_IO_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcam_io_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON53),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCAM_IO_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCAM_IO_EN_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vcam_io_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON53),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VCAM_IO_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VCAM_IO_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_vcam_io_ndis_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON54),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCAM_IO_NDIS_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCAM_IO_NDIS_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcam_io_on_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON54),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCAM_IO_ON_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_VCAM_IO_ON_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcam_io_ocfb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON54),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCAM_IO_OCFB_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCAM_IO_OCFB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcam_io_stb_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON54),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCAM_IO_STB_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCAM_IO_STB_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcam_io_cal(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON54),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCAM_IO_CAL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCAM_IO_CAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vm25_lp_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON55),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VM25_LP_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_VM25_LP_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vm25_lp_mode_set(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON55),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VM25_LP_MODE_SET_MASK),
                             (kal_uint32)(MT6392_PMIC_VM25_LP_MODE_SET_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vm25_mode(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON55),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VM25_MODE_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VM25_MODE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_vm25_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON55),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VM25_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VM25_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vm25_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON55),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VM25_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VM25_EN_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vm25_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON55),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VM25_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VM25_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_vm25_ndis_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON56),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VM25_NDIS_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VM25_NDIS_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vm25_plcur_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON56),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VM25_PLCUR_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VM25_PLCUR_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vm25_plcur_cal(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON56),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VM25_PLCUR_CAL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VM25_PLCUR_CAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vm25_ocfb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON56),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VM25_OCFB_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VM25_OCFB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vm25_cal(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON56),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VM25_CAL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VM25_CAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vm25_vosel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON56),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VM25_VOSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VM25_VOSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vefuse_lp_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON57),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VEFUSE_LP_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_VEFUSE_LP_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vefuse_lp_mode_set(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON57),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VEFUSE_LP_MODE_SET_MASK),
                             (kal_uint32)(MT6392_PMIC_VEFUSE_LP_MODE_SET_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vefuse_mode(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON57),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VEFUSE_MODE_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VEFUSE_MODE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_vefuse_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON57),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VEFUSE_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VEFUSE_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vefuse_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON57),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VEFUSE_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VEFUSE_EN_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_da_qi_vefuse_en(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON57),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VEFUSE_EN_MASK),
                           (kal_uint32)(MT6392_PMIC_DA_QI_VEFUSE_EN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_vefuse_ndis_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON58),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VEFUSE_NDIS_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VEFUSE_NDIS_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vefuse_plcur_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON58),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VEFUSE_PLCUR_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VEFUSE_PLCUR_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vefuse_plcur_cal(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON58),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VEFUSE_PLCUR_CAL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VEFUSE_PLCUR_CAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vefuse_vosel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON58),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VEFUSE_VOSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VEFUSE_VOSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vefuse_ocfb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON58),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VEFUSE_OCFB_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VEFUSE_OCFB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vefuse_cal(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON58),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VEFUSE_CAL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VEFUSE_CAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vgp1_oc_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON59),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VGP1_OC_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VGP1_OC_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcam_af_oc_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON59),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCAM_AF_OC_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCAM_AF_OC_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vemc_3v3_oc_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON59),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VEMC_3V3_OC_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VEMC_3V3_OC_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vmch_oc_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON59),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VMCH_OC_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VMCH_OC_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vmc_oc_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON59),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VMC_OC_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VMC_OC_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vusb_oc_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON59),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VUSB_OC_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VUSB_OC_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vio28_oc_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON59),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VIO28_OC_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VIO28_OC_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcn35_oc_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON59),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCN35_OC_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCN35_OC_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vm_oc_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON60),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VM_OC_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VM_OC_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcam_io_oc_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON60),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCAM_IO_OC_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCAM_IO_OC_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcamd_oc_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON60),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCAMD_OC_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCAMD_OC_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vcn18_oc_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON60),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VCN18_OC_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VCN18_OC_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vio18_oc_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON60),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VIO18_OC_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VIO18_OC_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vefuse_oc_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON60),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VEFUSE_OC_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VEFUSE_OC_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vm25_oc_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON60),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VM25_OC_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VM25_OC_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vgp2_oc_stbtd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON60),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VGP2_OC_STBTD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VGP2_OC_STBTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcn_1v8_on_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON61),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCN_1V8_ON_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_VCN_1V8_ON_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vm_on_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON61),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VM_ON_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_VM_ON_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vefuse_on_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON61),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VEFUSE_ON_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_VEFUSE_ON_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vm25_on_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON61),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VM25_ON_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_VM25_ON_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vusb_on_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON61),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VUSB_ON_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_VUSB_ON_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vio28_on_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON61),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VIO28_ON_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_VIO28_ON_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_vcn35_on_ctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON61),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_VCN35_ON_CTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_VCN35_ON_CTRL_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_rgs_qi_vcam_io_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON62),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VCAM_IO_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VCAM_IO_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_qi_vcamd_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON62),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VCAMD_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VCAMD_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_qi_vcn_1v8_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON62),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VCN_1V8_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VCN_1V8_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_qi_vio18_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON62),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VIO18_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VIO18_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_qi_vm_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON62),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VM_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VM_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_qi_vefuse_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON62),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VEFUSE_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VEFUSE_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_qi_vm25_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON62),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VM25_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VM25_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_qi_vgp2_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON62),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VGP2_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VGP2_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_qi_vgp1_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON62),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VGP1_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VGP1_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_qi_vcam_af_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON62),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VCAM_AF_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VCAM_AF_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_qi_vemc_3v3_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON62),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VEMC_3V3_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VEMC_3V3_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_qi_vmch_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON62),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VMCH_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VMCH_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_qi_vmc_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON62),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VMC_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VMC_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_qi_vusb_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON62),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VUSB_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VUSB_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_qi_vio28_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON62),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VIO28_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VIO28_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rgs_qi_vcn35_oc_status_debounce(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_DIGLDO_CON62),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VCN35_OC_STATUS_DEBOUNCE_MASK),
                           (kal_uint32)(MT6392_PMIC_RGS_QI_VCN35_OC_STATUS_DEBOUNCE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_qi_vgp1_dm_load(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_DIGLDO_CON63),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_QI_VGP1_DM_LOAD_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_QI_VGP1_DM_LOAD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_addr(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_ADDR_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_ADDR_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_prog(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_PROG_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_PROG_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_pkey(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_CON3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_PKEY_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_PKEY_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_rd_trig(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_CON4),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_RD_TRIG_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_RD_TRIG_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_prog_src(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_CON5),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_PROG_SRC_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_PROG_SRC_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_skip_efuse_out(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_CON5),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_SKIP_EFUSE_OUT_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_SKIP_EFUSE_OUT_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_rd_rdy_bypass(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_CON5),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_RD_RDY_BYPASS_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_RD_RDY_BYPASS_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_rg_efuse_rd_ack(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EFUSE_CON6),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_RD_ACK_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_RD_ACK_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_efuse_busy(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EFUSE_CON6),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_BUSY_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_BUSY_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_efuse_val_0_15(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_VAL_0_15),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_0_15_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_0_15_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_val_16_31(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_VAL_16_31),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_16_31_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_16_31_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_val_32_47(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_VAL_32_47),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_32_47_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_32_47_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_val_48_63(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_VAL_48_63),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_48_63_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_48_63_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_val_64_79(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_VAL_64_79),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_64_79_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_64_79_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_val_80_95(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_VAL_80_95),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_80_95_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_80_95_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_val_96_111(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_VAL_96_111),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_96_111_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_96_111_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_val_112_127(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_VAL_112_127),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_112_127_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_112_127_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_val_128_143(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_VAL_128_143),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_128_143_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_128_143_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_val_144_159(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_VAL_144_159),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_144_159_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_144_159_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_val_160_175(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_VAL_160_175),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_160_175_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_160_175_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_val_176_191(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_VAL_176_191),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_176_191_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_176_191_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_val_192_207(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_VAL_192_207),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_192_207_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_192_207_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_val_208_223(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_VAL_208_223),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_208_223_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_208_223_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_val_224_239(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_VAL_224_239),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_224_239_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_224_239_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_val_240_255(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_VAL_240_255),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_240_255_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_240_255_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_val_256_271(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_VAL_256_271),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_256_271_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_256_271_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_val_272_287(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_VAL_272_287),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_272_287_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_272_287_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_val_288_303(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_VAL_288_303),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_288_303_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_288_303_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_val_304_319(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_VAL_304_319),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_304_319_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_304_319_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_val_320_335(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_VAL_320_335),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_320_335_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_320_335_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_val_336_351(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_VAL_336_351),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_336_351_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_336_351_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_val_352_367(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_VAL_352_367),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_352_367_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_352_367_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_val_368_383(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_VAL_368_383),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_368_383_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_368_383_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_val_384_399(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_VAL_384_399),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_384_399_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_384_399_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_val_400_415(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_VAL_400_415),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_400_415_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_400_415_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_val_416_431(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_VAL_416_431),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_416_431_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_416_431_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_mix_eosc32_opt(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_RTC_MIX_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_MIX_EOSC32_OPT_MASK),
                             (kal_uint32)(MT6392_PMIC_MIX_EOSC32_OPT_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_mix_xosc32_stp_cpdtb(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_RTC_MIX_CON0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_MIX_XOSC32_STP_CPDTB_MASK),
                           (kal_uint32)(MT6392_PMIC_MIX_XOSC32_STP_CPDTB_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_mix_xosc32_stp_pwdb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_RTC_MIX_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_MIX_XOSC32_STP_PWDB_MASK),
                             (kal_uint32)(MT6392_PMIC_MIX_XOSC32_STP_PWDB_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_mix_xosc32_stp_lpdtb(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_RTC_MIX_CON0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_MIX_XOSC32_STP_LPDTB_MASK),
                           (kal_uint32)(MT6392_PMIC_MIX_XOSC32_STP_LPDTB_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_mix_xosc32_stp_lpden(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_RTC_MIX_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_MIX_XOSC32_STP_LPDEN_MASK),
                             (kal_uint32)(MT6392_PMIC_MIX_XOSC32_STP_LPDEN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_mix_xosc32_stp_lpdrst(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_RTC_MIX_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_MIX_XOSC32_STP_LPDRST_MASK),
                             (kal_uint32)(MT6392_PMIC_MIX_XOSC32_STP_LPDRST_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_mix_xosc32_stp_cali(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_RTC_MIX_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_MIX_XOSC32_STP_CALI_MASK),
                             (kal_uint32)(MT6392_PMIC_MIX_XOSC32_STP_CALI_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_stmp_mode(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_RTC_MIX_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_STMP_MODE_MASK),
                             (kal_uint32)(MT6392_PMIC_STMP_MODE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_mix_eosc32_stp_chop_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_RTC_MIX_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_MIX_EOSC32_STP_CHOP_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_MIX_EOSC32_STP_CHOP_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_mix_dcxo_stp_lvsh_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_RTC_MIX_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_MIX_DCXO_STP_LVSH_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_MIX_DCXO_STP_LVSH_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_mix_pmu_stp_ddlo_vrtc(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_RTC_MIX_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_MIX_PMU_STP_DDLO_VRTC_MASK),
                             (kal_uint32)(MT6392_PMIC_MIX_PMU_STP_DDLO_VRTC_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_mix_pmu_stp_ddlo_vrtc_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_RTC_MIX_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_MIX_PMU_STP_DDLO_VRTC_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_MIX_PMU_STP_DDLO_VRTC_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_mix_rtc_stp_xosc32_enb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_RTC_MIX_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_MIX_RTC_STP_XOSC32_ENB_MASK),
                             (kal_uint32)(MT6392_PMIC_MIX_RTC_STP_XOSC32_ENB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_mix_dcxo_stp_test_deglitch_mode(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_RTC_MIX_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_MIX_DCXO_STP_TEST_DEGLITCH_MODE_MASK),
                             (kal_uint32)(MT6392_PMIC_MIX_DCXO_STP_TEST_DEGLITCH_MODE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_mix_eosc32_stp_rsv(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_RTC_MIX_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_MIX_EOSC32_STP_RSV_MASK),
                             (kal_uint32)(MT6392_PMIC_MIX_EOSC32_STP_RSV_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_mix_eosc32_vct_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_RTC_MIX_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_MIX_EOSC32_VCT_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_MIX_EOSC32_VCT_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_mix_stp_bbwakeup(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_RTC_MIX_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_MIX_STP_BBWAKEUP_MASK),
                             (kal_uint32)(MT6392_PMIC_MIX_STP_BBWAKEUP_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_mix_stp_rtc_ddlo(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_RTC_MIX_CON1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_MIX_STP_RTC_DDLO_MASK),
                           (kal_uint32)(MT6392_PMIC_MIX_STP_RTC_DDLO_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_mix_rtc_xosc32_enb(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_RTC_MIX_CON1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_MIX_RTC_XOSC32_ENB_MASK),
                           (kal_uint32)(MT6392_PMIC_MIX_RTC_XOSC32_ENB_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_mix_efuse_xosc32_enb_opt(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_RTC_MIX_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_MIX_EFUSE_XOSC32_ENB_OPT_MASK),
                             (kal_uint32)(MT6392_PMIC_MIX_EFUSE_XOSC32_ENB_OPT_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_val_432_447(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_VAL_432_447),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_432_447_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_432_447_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_val_448_463(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_VAL_448_463),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_448_463_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_448_463_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_val_464_479(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_VAL_464_479),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_464_479_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_464_479_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_val_480_495(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_VAL_480_495),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_480_495_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_480_495_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_efuse_val_496_511(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_VAL_496_511),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_496_511_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_VAL_496_511_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_rg_efuse_dout_0_15(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EFUSE_DOUT_0_15),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_0_15_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_0_15_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_efuse_dout_16_31(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EFUSE_DOUT_16_31),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_16_31_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_16_31_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_efuse_dout_32_47(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EFUSE_DOUT_32_47),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_32_47_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_32_47_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_efuse_dout_48_63(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EFUSE_DOUT_48_63),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_48_63_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_48_63_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_efuse_dout_64_79(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EFUSE_DOUT_64_79),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_64_79_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_64_79_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_efuse_dout_80_95(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EFUSE_DOUT_80_95),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_80_95_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_80_95_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_efuse_dout_96_111(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EFUSE_DOUT_96_111),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_96_111_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_96_111_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_efuse_dout_112_127(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EFUSE_DOUT_112_127),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_112_127_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_112_127_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_efuse_dout_128_143(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EFUSE_DOUT_128_143),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_128_143_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_128_143_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_efuse_dout_144_159(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EFUSE_DOUT_144_159),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_144_159_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_144_159_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_efuse_dout_160_175(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EFUSE_DOUT_160_175),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_160_175_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_160_175_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_efuse_dout_176_191(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EFUSE_DOUT_176_191),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_176_191_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_176_191_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_efuse_dout_192_207(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EFUSE_DOUT_192_207),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_192_207_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_192_207_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_efuse_dout_208_223(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EFUSE_DOUT_208_223),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_208_223_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_208_223_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_efuse_dout_224_239(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EFUSE_DOUT_224_239),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_224_239_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_224_239_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_efuse_dout_240_255(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EFUSE_DOUT_240_255),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_240_255_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_240_255_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_efuse_dout_256_271(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EFUSE_DOUT_256_271),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_256_271_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_256_271_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_efuse_dout_272_287(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EFUSE_DOUT_272_287),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_272_287_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_272_287_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_efuse_dout_288_303(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EFUSE_DOUT_288_303),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_288_303_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_288_303_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_efuse_dout_304_319(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EFUSE_DOUT_304_319),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_304_319_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_304_319_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_efuse_dout_320_335(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EFUSE_DOUT_320_335),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_320_335_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_320_335_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_efuse_dout_336_351(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EFUSE_DOUT_336_351),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_336_351_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_336_351_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_efuse_dout_352_367(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EFUSE_DOUT_352_367),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_352_367_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_352_367_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_efuse_dout_368_383(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EFUSE_DOUT_368_383),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_368_383_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_368_383_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_efuse_dout_384_399(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EFUSE_DOUT_384_399),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_384_399_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_384_399_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_efuse_dout_400_415(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EFUSE_DOUT_400_415),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_400_415_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_400_415_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_efuse_dout_416_431(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EFUSE_DOUT_416_431),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_416_431_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_416_431_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_efuse_dout_432_447(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EFUSE_DOUT_432_447),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_432_447_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_432_447_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_efuse_dout_448_463(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EFUSE_DOUT_448_463),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_448_463_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_448_463_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_efuse_dout_464_479(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EFUSE_DOUT_464_479),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_464_479_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_464_479_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_efuse_dout_480_495(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EFUSE_DOUT_480_495),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_480_495_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_480_495_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_rg_efuse_dout_496_511(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_EFUSE_DOUT_496_511),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_496_511_MASK),
                           (kal_uint32)(MT6392_PMIC_RG_EFUSE_DOUT_496_511_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_efuse_rd_pkey(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_RD_PKEY_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_EFUSE_RD_PKEY_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_otp_pdin(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_OTP_PDIN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_OTP_PDIN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_otp_ptm(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_EFUSE_CON9),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_OTP_PTM_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_OTP_PTM_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_auxadc_adc_out_ch0(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_CH0_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_CH0_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_rdy_ch0(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_CH0_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_CH0_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_out_ch1(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_CH1_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_CH1_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_rdy_ch1(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_CH1_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_CH1_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_out_ch2(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC2),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_CH2_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_CH2_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_rdy_ch2(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC2),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_CH2_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_CH2_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_out_ch3(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC3),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_CH3_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_CH3_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_rdy_ch3(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC3),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_CH3_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_CH3_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_out_ch4(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC4),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_CH4_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_CH4_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_rdy_ch4(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC4),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_CH4_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_CH4_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_out_ch5(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC5),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_CH5_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_CH5_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_rdy_ch5(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC5),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_CH5_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_CH5_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_out_ch6(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC6),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_CH6_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_CH6_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_rdy_ch6(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC6),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_CH6_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_CH6_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_out_ch7(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC7),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_CH7_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_CH7_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_rdy_ch7(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC7),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_CH7_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_CH7_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_out_ch8(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC8),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_CH8_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_CH8_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_rdy_ch8(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC8),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_CH8_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_CH8_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_out_ch9(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC9),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_CH9_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_CH9_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_rdy_ch9(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC9),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_CH9_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_CH9_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_out_ch10(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC10),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_CH10_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_CH10_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_rdy_ch10(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC10),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_CH10_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_CH10_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_out_ch11(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC11),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_CH11_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_CH11_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_rdy_ch11(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC11),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_CH11_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_CH11_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_out_ch12_15(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC12),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_CH12_15_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_CH12_15_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_rdy_ch12_15(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC12),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_CH12_15_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_CH12_15_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_out_thr_hw(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC13),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_THR_HW_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_THR_HW_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_rdy_thr_hw(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC13),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_THR_HW_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_THR_HW_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_out_lbat(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC14),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_LBAT_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_LBAT_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_rdy_lbat(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC14),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_LBAT_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_LBAT_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_out_wakeup_pchr(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC15),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_WAKEUP_PCHR_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_WAKEUP_PCHR_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_rdy_wakeup_pchr(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC15),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_WAKEUP_PCHR_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_WAKEUP_PCHR_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_out_wakeup_swchr(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC16),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_WAKEUP_SWCHR_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_WAKEUP_SWCHR_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_rdy_wakeup_swchr(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC16),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_WAKEUP_SWCHR_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_WAKEUP_SWCHR_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_out_ch0_by_ap(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC17),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_CH0_BY_AP_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_CH0_BY_AP_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_rdy_ch0_by_ap(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC17),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_CH0_BY_AP_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_CH0_BY_AP_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_out_ch1_by_ap(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC18),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_CH1_BY_AP_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_CH1_BY_AP_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_rdy_ch1_by_ap(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC18),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_CH1_BY_AP_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_CH1_BY_AP_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_out_raw(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC19),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_RAW_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_RAW_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_out_nag(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC20),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_NAG_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_NAG_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_rdy_nag(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC20),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_NAG_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_NAG_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_out_typec_h(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC21),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_TYPEC_H_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_TYPEC_H_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_rdy_typec_h(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC21),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_TYPEC_H_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_TYPEC_H_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_out_typec_l(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC22),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_TYPEC_L_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_OUT_TYPEC_L_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_rdy_typec_l(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_ADC22),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_TYPEC_L_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_TYPEC_L_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_busy_in(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_STA0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_BUSY_IN_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_BUSY_IN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_busy_in_lbat(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_STA0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_BUSY_IN_LBAT_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_BUSY_IN_LBAT_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_busy_in_wakeup(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_STA0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_BUSY_IN_WAKEUP_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_BUSY_IN_WAKEUP_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_busy_in_typec_h(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_STA1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_BUSY_IN_TYPEC_H_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_BUSY_IN_TYPEC_H_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_busy_in_typec_l(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_STA1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_BUSY_IN_TYPEC_L_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_BUSY_IN_TYPEC_L_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_busy_in_nag(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_STA1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_BUSY_IN_NAG_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_BUSY_IN_NAG_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_busy_in_share(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_STA1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_BUSY_IN_SHARE_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_BUSY_IN_SHARE_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_adc_busy_in_thr_hw(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_STA1),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_BUSY_IN_THR_HW_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_ADC_BUSY_IN_THR_HW_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_auxadc_rqst_ch0(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_RQST0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_RQST_CH0_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_RQST_CH0_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_rqst_ch1(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_RQST0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_RQST_CH1_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_RQST_CH1_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_rqst_ch2(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_RQST0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_RQST_CH2_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_RQST_CH2_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_rqst_ch3(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_RQST0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_RQST_CH3_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_RQST_CH3_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_rqst_ch4(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_RQST0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_RQST_CH4_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_RQST_CH4_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_rqst_ch5(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_RQST0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_RQST_CH5_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_RQST_CH5_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_rqst_ch6(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_RQST0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_RQST_CH6_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_RQST_CH6_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_rqst_ch7(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_RQST0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_RQST_CH7_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_RQST_CH7_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_rqst_ch8(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_RQST0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_RQST_CH8_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_RQST_CH8_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_rqst_ch9(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_RQST0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_RQST_CH9_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_RQST_CH9_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_rqst_ch10(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_RQST0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_RQST_CH10_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_RQST_CH10_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_rqst_ch11(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_RQST0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_RQST_CH11_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_RQST_CH11_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_rqst_ch12(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_RQST0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_RQST_CH12_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_RQST_CH12_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_rqst_ch13(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_RQST0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_RQST_CH13_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_RQST_CH13_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_rqst_ch14(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_RQST0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_RQST_CH14_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_RQST_CH14_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_rqst_ch15(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_RQST0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_RQST_CH15_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_RQST_CH15_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_ck_on_extd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_CK_ON_EXTD_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_CK_ON_EXTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_adc_pwdb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_ADC_PWDB_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_ADC_PWDB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_adc_pwdb_swctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_ADC_PWDB_SWCTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_ADC_PWDB_SWCTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_strup_ck_on_enb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_STRUP_CK_ON_ENB_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_STRUP_CK_ON_ENB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_adc_rdy_wakeup_clr(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_WAKEUP_CLR_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_ADC_RDY_WAKEUP_CLR_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_12m_ck_aon(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_12M_CK_AON_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_12M_CK_AON_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_ck_aon(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_CK_AON_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_CK_AON_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_avg_num_small(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_AVG_NUM_SMALL_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_AVG_NUM_SMALL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_avg_num_large(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_AVG_NUM_LARGE_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_AVG_NUM_LARGE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_spl_num(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_SPL_NUM_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_SPL_NUM_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_avg_num_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_AVG_NUM_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_AVG_NUM_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_avg_num_sel_share(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_AVG_NUM_SEL_SHARE_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_AVG_NUM_SEL_SHARE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_avg_num_sel_lbat(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_AVG_NUM_SEL_LBAT_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_AVG_NUM_SEL_LBAT_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_avg_num_sel_wakeup(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_AVG_NUM_SEL_WAKEUP_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_AVG_NUM_SEL_WAKEUP_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_spl_num_large(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_SPL_NUM_LARGE_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_SPL_NUM_LARGE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_spl_num_sleep(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON4),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_SPL_NUM_SLEEP_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_SPL_NUM_SLEEP_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_spl_num_sleep_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON4),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_SPL_NUM_SLEEP_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_SPL_NUM_SLEEP_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_spl_num_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON5),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_SPL_NUM_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_SPL_NUM_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_spl_num_sel_share(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON5),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_SPL_NUM_SEL_SHARE_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_SPL_NUM_SEL_SHARE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_spl_num_sel_lbat(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON5),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_SPL_NUM_SEL_LBAT_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_SPL_NUM_SEL_LBAT_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_spl_num_sel_wakeup(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON5),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_SPL_NUM_SEL_WAKEUP_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_SPL_NUM_SEL_WAKEUP_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_trim_ch0_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON6),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TRIM_CH0_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TRIM_CH0_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_trim_ch1_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON6),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TRIM_CH1_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TRIM_CH1_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_trim_ch2_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON6),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TRIM_CH2_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TRIM_CH2_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_trim_ch3_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON6),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TRIM_CH3_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TRIM_CH3_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_trim_ch4_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON6),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TRIM_CH4_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TRIM_CH4_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_trim_ch5_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON6),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TRIM_CH5_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TRIM_CH5_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_trim_ch6_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON6),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TRIM_CH6_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TRIM_CH6_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_trim_ch7_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON6),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TRIM_CH7_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TRIM_CH7_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_trim_ch8_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TRIM_CH8_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TRIM_CH8_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_trim_ch9_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TRIM_CH9_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TRIM_CH9_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_trim_ch10_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TRIM_CH10_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TRIM_CH10_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_trim_ch11_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TRIM_CH11_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TRIM_CH11_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_adc_2s_comp_enb(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_ADC_2S_COMP_ENB_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_ADC_2S_COMP_ENB_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_adc_trim_comp(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON7),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_ADC_TRIM_COMP_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_ADC_TRIM_COMP_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_sw_gain_trim(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON8),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_SW_GAIN_TRIM_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_SW_GAIN_TRIM_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_sw_offset_trim(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON9),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_SW_OFFSET_TRIM_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_SW_OFFSET_TRIM_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_rng_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_RNG_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_RNG_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_data_reuse_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_DATA_REUSE_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_DATA_REUSE_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_test_mode(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TEST_MODE_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TEST_MODE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_bit_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_BIT_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_BIT_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_start_sw(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_START_SW_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_START_SW_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_start_swctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_START_SWCTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_START_SWCTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_ts_vbe_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TS_VBE_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TS_VBE_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_ts_vbe_sel_swctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TS_VBE_SEL_SWCTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TS_VBE_SEL_SWCTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_vbuf_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_VBUF_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_VBUF_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_vbuf_en_swctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_VBUF_EN_SWCTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_VBUF_EN_SWCTRL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_out_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON10),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_OUT_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_OUT_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_da_dac(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON11),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_DA_DAC_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_DA_DAC_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_da_dac_swctrl(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON11),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_DA_DAC_SWCTRL_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_DA_DAC_SWCTRL_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_ad_auxadc_comp(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_CON11),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AD_AUXADC_COMP_MASK),
                           (kal_uint32)(MT6392_PMIC_AD_AUXADC_COMP_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_rg_vbuf_exten(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VBUF_EXTEN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VBUF_EXTEN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vbuf_calen(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VBUF_CALEN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VBUF_CALEN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vbuf_byp(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VBUF_BYP_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VBUF_BYP_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_aux_rsv(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_AUX_RSV_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_AUX_RSV_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_auxadc_cali(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_AUXADC_CALI_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_AUXADC_CALI_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vbuf_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VBUF_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VBUF_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_rg_vbe_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON12),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_RG_VBE_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_RG_VBE_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_adcin_vsen_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON13),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_ADCIN_VSEN_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_ADCIN_VSEN_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_adcin_vbat_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON13),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_ADCIN_VBAT_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_ADCIN_VBAT_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_adcin_vsen_mux_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON13),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_ADCIN_VSEN_MUX_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_ADCIN_VSEN_MUX_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_adcin_vsen_ext_baton_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON13),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_ADCIN_VSEN_EXT_BATON_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_ADCIN_VSEN_EXT_BATON_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_adcin_chr_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON13),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_ADCIN_CHR_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_ADCIN_CHR_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_adcin_baton_tdet_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON13),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_ADCIN_BATON_TDET_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_ADCIN_BATON_TDET_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_dig0_rsv0(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON13),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_DIG0_RSV0_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_DIG0_RSV0_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_chsel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON13),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_CHSEL_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_CHSEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_swctrl_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON13),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_SWCTRL_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_SWCTRL_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_source_lbat_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON14),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_SOURCE_LBAT_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_SOURCE_LBAT_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_dac_extd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON14),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_DAC_EXTD_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_DAC_EXTD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_dac_extd_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON14),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_DAC_EXTD_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_DAC_EXTD_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_pmu_thr_pdn_sw(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON15),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_PMU_THR_PDN_SW_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_PMU_THR_PDN_SW_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_pmu_thr_pdn_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON15),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_PMU_THR_PDN_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_PMU_THR_PDN_SEL_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_auxadc_pmu_thr_pdn_status(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_CON15),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_PMU_THR_PDN_STATUS_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_PMU_THR_PDN_STATUS_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_auxadc_start_shade_num(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON16),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_START_SHADE_NUM_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_START_SHADE_NUM_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_start_shade_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON16),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_START_SHADE_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_START_SHADE_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_start_shade_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_CON16),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_START_SHADE_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_START_SHADE_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_autorpt_prd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_AUTORPT0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_AUTORPT_PRD_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_AUTORPT_PRD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_autorpt_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_AUTORPT0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_AUTORPT_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_AUTORPT_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_lbat_debt_max(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_LBAT0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_LBAT_DEBT_MAX_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_LBAT_DEBT_MAX_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_lbat_debt_min(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_LBAT0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_LBAT_DEBT_MIN_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_LBAT_DEBT_MIN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_lbat_det_prd_15_0(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_LBAT1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_LBAT_DET_PRD_15_0_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_LBAT_DET_PRD_15_0_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_lbat_det_prd_19_16(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_LBAT2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_LBAT_DET_PRD_19_16_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_LBAT_DET_PRD_19_16_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_lbat_volt_max(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_LBAT3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_LBAT_VOLT_MAX_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_LBAT_VOLT_MAX_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_lbat_irq_en_max(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_LBAT3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_LBAT_IRQ_EN_MAX_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_LBAT_IRQ_EN_MAX_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_lbat_en_max(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_LBAT3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_LBAT_EN_MAX_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_LBAT_EN_MAX_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_auxadc_lbat_max_irq_b(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_LBAT3),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_LBAT_MAX_IRQ_B_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_LBAT_MAX_IRQ_B_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_auxadc_lbat_volt_min(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_LBAT4),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_LBAT_VOLT_MIN_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_LBAT_VOLT_MIN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_lbat_irq_en_min(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_LBAT4),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_LBAT_IRQ_EN_MIN_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_LBAT_IRQ_EN_MIN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_lbat_en_min(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_LBAT4),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_LBAT_EN_MIN_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_LBAT_EN_MIN_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_auxadc_lbat_min_irq_b(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_LBAT4),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_LBAT_MIN_IRQ_B_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_LBAT_MIN_IRQ_B_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_lbat_debounce_count_max(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_LBAT5),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_LBAT_DEBOUNCE_COUNT_MAX_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_LBAT_DEBOUNCE_COUNT_MAX_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_lbat_debounce_count_min(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_LBAT6),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_LBAT_DEBOUNCE_COUNT_MIN_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_LBAT_DEBOUNCE_COUNT_MIN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_auxadc_thr_debt_max(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_THR0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_THR_DEBT_MAX_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_THR_DEBT_MAX_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_thr_debt_min(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_THR0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_THR_DEBT_MIN_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_THR_DEBT_MIN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_thr_det_prd_15_0(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_THR1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_THR_DET_PRD_15_0_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_THR_DET_PRD_15_0_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_thr_det_prd_19_16(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_THR2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_THR_DET_PRD_19_16_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_THR_DET_PRD_19_16_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_thr_volt_max(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_THR3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_THR_VOLT_MAX_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_THR_VOLT_MAX_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_thr_irq_en_max(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_THR3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_THR_IRQ_EN_MAX_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_THR_IRQ_EN_MAX_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_thr_en_max(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_THR3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_THR_EN_MAX_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_THR_EN_MAX_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_auxadc_thr_max_irq_b(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_THR3),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_THR_MAX_IRQ_B_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_THR_MAX_IRQ_B_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_auxadc_thr_volt_min(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_THR4),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_THR_VOLT_MIN_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_THR_VOLT_MIN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_thr_irq_en_min(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_THR4),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_THR_IRQ_EN_MIN_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_THR_IRQ_EN_MIN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_thr_en_min(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_THR4),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_THR_EN_MIN_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_THR_EN_MIN_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_auxadc_thr_min_irq_b(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_THR4),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_THR_MIN_IRQ_B_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_THR_MIN_IRQ_B_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_thr_debounce_count_max(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_THR5),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_THR_DEBOUNCE_COUNT_MAX_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_THR_DEBOUNCE_COUNT_MAX_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_thr_debounce_count_min(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_THR6),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_THR_DEBOUNCE_COUNT_MIN_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_THR_DEBOUNCE_COUNT_MIN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_efuse_gain_ch4_trim(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_EFUSE0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_EFUSE_GAIN_CH4_TRIM_MASK),
                             (kal_uint32)(MT6392_PMIC_EFUSE_GAIN_CH4_TRIM_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_efuse_offset_ch4_trim(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_EFUSE1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_EFUSE_OFFSET_CH4_TRIM_MASK),
                             (kal_uint32)(MT6392_PMIC_EFUSE_OFFSET_CH4_TRIM_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_efuse_gain_ch0_trim(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_EFUSE2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_EFUSE_GAIN_CH0_TRIM_MASK),
                             (kal_uint32)(MT6392_PMIC_EFUSE_GAIN_CH0_TRIM_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_efuse_offset_ch0_trim(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_EFUSE3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_EFUSE_OFFSET_CH0_TRIM_MASK),
                             (kal_uint32)(MT6392_PMIC_EFUSE_OFFSET_CH0_TRIM_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_efuse_gain_ch7_trim(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_EFUSE4),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_EFUSE_GAIN_CH7_TRIM_MASK),
                             (kal_uint32)(MT6392_PMIC_EFUSE_GAIN_CH7_TRIM_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_efuse_offset_ch7_trim(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_EFUSE5),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_EFUSE_OFFSET_CH7_TRIM_MASK),
                             (kal_uint32)(MT6392_PMIC_EFUSE_OFFSET_CH7_TRIM_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_nag_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_NAG_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_NAG_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_NAG_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_nag_clr(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_NAG_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_NAG_CLR_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_NAG_CLR_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_nag_vbat1_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_NAG_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_NAG_VBAT1_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_NAG_VBAT1_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_nag_prd_22_16(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_NAG_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_NAG_PRD_22_16_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_NAG_PRD_22_16_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_nag_irq_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_NAG_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_NAG_IRQ_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_NAG_IRQ_EN_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_auxadc_nag_c_dltv_irq(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_NAG_0),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_NAG_C_DLTV_IRQ_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_NAG_C_DLTV_IRQ_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_auxadc_nag_zcv(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_NAG_1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_NAG_ZCV_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_NAG_ZCV_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_nag_c_dltv_th_15_0(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_NAG_2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_NAG_C_DLTV_TH_15_0_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_NAG_C_DLTV_TH_15_0_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_nag_c_dltv_th_26_16(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_NAG_3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_NAG_C_DLTV_TH_26_16_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_NAG_C_DLTV_TH_26_16_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_auxadc_nag_cnt_15_0(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_NAG_4),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_NAG_CNT_15_0_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_NAG_CNT_15_0_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_nag_cnt_25_16(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_NAG_5),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_NAG_CNT_25_16_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_NAG_CNT_25_16_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_nag_dltv(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_NAG_6),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_NAG_DLTV_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_NAG_DLTV_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_nag_c_dltv_15_0(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_NAG_7),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_NAG_C_DLTV_15_0_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_NAG_C_DLTV_15_0_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_nag_c_dltv_26_16(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_NAG_8),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_NAG_C_DLTV_26_16_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_NAG_C_DLTV_26_16_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_auxadc_typec_h_debt_max(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_TYPEC_H_1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_H_DEBT_MAX_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_H_DEBT_MAX_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_typec_h_debt_min(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_TYPEC_H_1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_H_DEBT_MIN_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_H_DEBT_MIN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_typec_h_det_prd_15_0(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_TYPEC_H_2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_H_DET_PRD_15_0_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_H_DET_PRD_15_0_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_typec_h_det_prd_19_16(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_TYPEC_H_3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_H_DET_PRD_19_16_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_H_DET_PRD_19_16_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_typec_h_volt_max(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_TYPEC_H_4),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_H_VOLT_MAX_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_H_VOLT_MAX_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_typec_h_irq_en_max(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_TYPEC_H_4),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_H_IRQ_EN_MAX_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_H_IRQ_EN_MAX_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_typec_h_en_max(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_TYPEC_H_4),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_H_EN_MAX_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_H_EN_MAX_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_auxadc_typec_h_max_irq_b(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_TYPEC_H_4),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_H_MAX_IRQ_B_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_H_MAX_IRQ_B_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_auxadc_typec_h_volt_min(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_TYPEC_H_5),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_H_VOLT_MIN_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_H_VOLT_MIN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_typec_h_irq_en_min(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_TYPEC_H_5),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_H_IRQ_EN_MIN_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_H_IRQ_EN_MIN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_typec_h_en_min(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_TYPEC_H_5),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_H_EN_MIN_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_H_EN_MIN_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_auxadc_typec_h_min_irq_b(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_TYPEC_H_5),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_H_MIN_IRQ_B_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_H_MIN_IRQ_B_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_typec_h_debounce_count_max(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_TYPEC_H_6),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_H_DEBOUNCE_COUNT_MAX_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_H_DEBOUNCE_COUNT_MAX_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_typec_h_debounce_count_min(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_TYPEC_H_7),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_H_DEBOUNCE_COUNT_MIN_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_H_DEBOUNCE_COUNT_MIN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_auxadc_typec_l_debt_max(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_TYPEC_L_1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_L_DEBT_MAX_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_L_DEBT_MAX_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_typec_l_debt_min(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_TYPEC_L_1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_L_DEBT_MIN_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_L_DEBT_MIN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_typec_l_det_prd_15_0(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_TYPEC_L_2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_L_DET_PRD_15_0_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_L_DET_PRD_15_0_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_typec_l_det_prd_19_16(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_TYPEC_L_3),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_L_DET_PRD_19_16_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_L_DET_PRD_19_16_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_typec_l_volt_max(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_TYPEC_L_4),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_L_VOLT_MAX_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_L_VOLT_MAX_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_typec_l_irq_en_max(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_TYPEC_L_4),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_L_IRQ_EN_MAX_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_L_IRQ_EN_MAX_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_typec_l_en_max(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_TYPEC_L_4),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_L_EN_MAX_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_L_EN_MAX_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_auxadc_typec_l_max_irq_b(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_TYPEC_L_4),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_L_MAX_IRQ_B_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_L_MAX_IRQ_B_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_auxadc_typec_l_volt_min(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_TYPEC_L_5),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_L_VOLT_MIN_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_L_VOLT_MIN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_typec_l_irq_en_min(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_TYPEC_L_5),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_L_IRQ_EN_MIN_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_L_IRQ_EN_MIN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_auxadc_typec_l_en_min(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_TYPEC_L_5),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_L_EN_MIN_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_L_EN_MIN_SHIFT)
	                         );
  pmic_unlock();
}

kal_uint32 mt6392_upmu_get_auxadc_typec_l_min_irq_b(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_TYPEC_L_5),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_L_MIN_IRQ_B_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_L_MIN_IRQ_B_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_typec_l_debounce_count_max(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_TYPEC_L_6),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_L_DEBOUNCE_COUNT_MAX_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_L_DEBOUNCE_COUNT_MAX_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

kal_uint32 mt6392_upmu_get_auxadc_typec_l_debounce_count_min(void)
{
  kal_uint32 ret=0;
  kal_uint32 val=0;

  pmic_lock();
  ret=pmic_read_interface( (kal_uint32)(MT6392_AUXADC_TYPEC_L_7),
                           (&val),
                           (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_L_DEBOUNCE_COUNT_MIN_MASK),
                           (kal_uint32)(MT6392_PMIC_AUXADC_TYPEC_L_DEBOUNCE_COUNT_MIN_SHIFT)
	                       );
  pmic_unlock();

  return val;
}

void mt6392_upmu_set_auxadc_nag_prd_15_0(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_AUXADC_NAG_9),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_AUXADC_NAG_PRD_15_0_MASK),
                             (kal_uint32)(MT6392_PMIC_AUXADC_NAG_PRD_15_0_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_type_c_phy_rg_cc_rp_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_PHY_RG_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_TYPE_C_PHY_RG_CC_RP_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_TYPE_C_PHY_RG_CC_RP_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_phy_rg_cc_v2i_bypass(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_PHY_RG_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_PHY_RG_CC_V2I_BYPASS_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_PHY_RG_CC_V2I_BYPASS_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_phy_rg_cc_mpx_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_PHY_RG_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_PHY_RG_CC_MPX_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_PHY_RG_CC_MPX_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_phy_rg_cc_reserve(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_PHY_RG_CC_RESERVE_CSR),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_PHY_RG_CC_RESERVE_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_PHY_RG_CC_RESERVE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_vcmp_cc2_sw_sel_st_cnt_val(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_VCMP_CTRL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_VCMP_CC2_SW_SEL_ST_CNT_VAL_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_VCMP_CC2_SW_SEL_ST_CNT_VAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_vcmp_bias_en_st_cnt_val(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_VCMP_CTRL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_VCMP_BIAS_EN_ST_CNT_VAL_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_VCMP_BIAS_EN_ST_CNT_VAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_vcmp_dac_en_st_cnt_val(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_VCMP_CTRL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_VCMP_DAC_EN_ST_CNT_VAL_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_VCMP_DAC_EN_ST_CNT_VAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_port_support_role(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CTRL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_PORT_SUPPORT_ROLE_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_PORT_SUPPORT_ROLE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_adc_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CTRL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_ADC_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_ADC_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_acc_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CTRL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_ACC_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_ACC_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_audio_acc_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CTRL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_AUDIO_ACC_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_AUDIO_ACC_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_debug_acc_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CTRL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_DEBUG_ACC_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_DEBUG_ACC_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_try_src_st_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CTRL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_TRY_SRC_ST_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_TRY_SRC_ST_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_try_snk_st_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CTRL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_TRY_SNK_ST_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_TRY_SNK_ST_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_attach_src_2_try_wait_snk_st_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CTRL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_ATTACH_SRC_2_TRY_WAIT_SNK_ST_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_ATTACH_SRC_2_TRY_WAIT_SNK_ST_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_pd2cc_det_disable_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CTRL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_PD2CC_DET_DISABLE_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_PD2CC_DET_DISABLE_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_attach_src_open_pddebounce_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CTRL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_ATTACH_SRC_OPEN_PDDEBOUNCE_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_ATTACH_SRC_OPEN_PDDEBOUNCE_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_disable_st_rd_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CTRL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_DISABLE_ST_RD_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_DISABLE_ST_RD_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_da_cc_saclk_sw_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CTRL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_DA_CC_SACLK_SW_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_DA_CC_SACLK_SW_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_try_wait_src2attach_src_deb_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CTRL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_TRY_WAIT_SRC2ATTACH_SRC_DEB_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_TRY_WAIT_SRC2ATTACH_SRC_DEB_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_type_c_sw_vbus_present(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SW_CTRL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_TYPE_C_SW_VBUS_PRESENT_MASK),
                             (kal_uint32)(MT6392_PMIC_TYPE_C_SW_VBUS_PRESENT_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_type_c_sw_vbus_det_dis(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SW_CTRL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_TYPE_C_SW_VBUS_DET_DIS_MASK),
                             (kal_uint32)(MT6392_PMIC_TYPE_C_SW_VBUS_DET_DIS_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_type_c_sw_cc_det_dis(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SW_CTRL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_TYPE_C_SW_CC_DET_DIS_MASK),
                             (kal_uint32)(MT6392_PMIC_TYPE_C_SW_CC_DET_DIS_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_type_c_sw_pd_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SW_CTRL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_TYPE_C_SW_PD_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_TYPE_C_SW_PD_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_cc_vol_periodic_meas_val(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_VOL_PERIODIC_MEAS_VAL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_VOL_PERIODIC_MEAS_VAL_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_VOL_PERIODIC_MEAS_VAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_cc_vol_cc_debounce_cnt_val(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_VOL_DEBOUCE_CNT_VAL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_VOL_CC_DEBOUNCE_CNT_VAL_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_VOL_CC_DEBOUNCE_CNT_VAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_cc_vol_pd_debounce_cnt_val(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_VOL_DEBOUCE_CNT_VAL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_VOL_PD_DEBOUNCE_CNT_VAL_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_VOL_PD_DEBOUNCE_CNT_VAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_drp_src_cnt_val_0(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_DRP_SRC_CNT_VAL_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_DRP_SRC_CNT_VAL_0_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_DRP_SRC_CNT_VAL_0_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_drp_snk_cnt_val_0(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_DRP_SNK_CNT_VAL_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_DRP_SNK_CNT_VAL_0_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_DRP_SNK_CNT_VAL_0_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_drp_try_cnt_val_0(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_DRP_TRY_CNT_VAL_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_DRP_TRY_CNT_VAL_0_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_DRP_TRY_CNT_VAL_0_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_cc_src_vopen_default_dac_val(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SRC_DEFAULT_DAC_VAL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_SRC_VOPEN_DEFAULT_DAC_VAL_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_SRC_VOPEN_DEFAULT_DAC_VAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_cc_src_vrd_default_dac_val(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SRC_DEFAULT_DAC_VAL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_SRC_VRD_DEFAULT_DAC_VAL_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_SRC_VRD_DEFAULT_DAC_VAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_cc_src_vopen_15_dac_val(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SRC_15_DAC_VAL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_SRC_VOPEN_15_DAC_VAL_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_SRC_VOPEN_15_DAC_VAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_cc_src_vrd_15_dac_val(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SRC_15_DAC_VAL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_SRC_VRD_15_DAC_VAL_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_SRC_VRD_15_DAC_VAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_cc_src_vopen_30_dac_val(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SRC_30_DAC_VAL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_SRC_VOPEN_30_DAC_VAL_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_SRC_VOPEN_30_DAC_VAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_cc_src_vrd_30_dac_val(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SRC_30_DAC_VAL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_SRC_VRD_30_DAC_VAL_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_SRC_VRD_30_DAC_VAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_cc_snk_vrp30_dac_val(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SNK_DAC_VAL_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_SNK_VRP30_DAC_VAL_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_SNK_VRP30_DAC_VAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_cc_snk_vrp15_dac_val(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SNK_DAC_VAL_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_SNK_VRP15_DAC_VAL_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_SNK_VRP15_DAC_VAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_cc_snk_vrpusb_dac_val(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SNK_DAC_VAL_1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_SNK_VRPUSB_DAC_VAL_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_SNK_VRPUSB_DAC_VAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_cc_ent_attach_src_intr_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_INTR_EN_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_ATTACH_SRC_INTR_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_ATTACH_SRC_INTR_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_cc_ent_attach_snk_intr_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_INTR_EN_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_ATTACH_SNK_INTR_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_ATTACH_SNK_INTR_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_cc_ent_audio_acc_intr_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_INTR_EN_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_AUDIO_ACC_INTR_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_AUDIO_ACC_INTR_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_cc_ent_disable_intr_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_INTR_EN_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_DISABLE_INTR_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_DISABLE_INTR_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_cc_ent_unattach_src_intr_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_INTR_EN_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_UNATTACH_SRC_INTR_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_UNATTACH_SRC_INTR_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_cc_ent_unattach_snk_intr_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_INTR_EN_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_UNATTACH_SNK_INTR_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_UNATTACH_SNK_INTR_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_cc_ent_attach_wait_src_intr_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_INTR_EN_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_ATTACH_WAIT_SRC_INTR_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_ATTACH_WAIT_SRC_INTR_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_cc_ent_attach_wait_snk_intr_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_INTR_EN_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_ATTACH_WAIT_SNK_INTR_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_ATTACH_WAIT_SNK_INTR_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_cc_ent_try_src_intr_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_INTR_EN_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_TRY_SRC_INTR_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_TRY_SRC_INTR_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_cc_ent_try_wait_snk_intr_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_INTR_EN_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_TRY_WAIT_SNK_INTR_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_TRY_WAIT_SNK_INTR_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_cc_ent_unattach_acc_intr_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_INTR_EN_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_UNATTACH_ACC_INTR_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_UNATTACH_ACC_INTR_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_cc_ent_attach_wait_acc_intr_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_INTR_EN_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_ATTACH_WAIT_ACC_INTR_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_ATTACH_WAIT_ACC_INTR_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_cc_ent_try_snk_intr_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_INTR_EN_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_TRY_SNK_INTR_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_TRY_SNK_INTR_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_cc_ent_try_wait_src_intr_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_INTR_EN_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_TRY_WAIT_SRC_INTR_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_TRY_WAIT_SRC_INTR_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_cc_ent_dbg_acc_src_intr_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_INTR_EN_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_DBG_ACC_SRC_INTR_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_DBG_ACC_SRC_INTR_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_cc_ent_dbg_acc_snk_intr_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_INTR_EN_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_DBG_ACC_SNK_INTR_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_DBG_ACC_SNK_INTR_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_cc_ent_snk_pwr_idle_intr_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_INTR_EN_2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_SNK_PWR_IDLE_INTR_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_SNK_PWR_IDLE_INTR_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_cc_ent_snk_pwr_default_intr_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_INTR_EN_2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_SNK_PWR_DEFAULT_INTR_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_SNK_PWR_DEFAULT_INTR_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_cc_ent_snk_pwr_15_intr_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_INTR_EN_2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_SNK_PWR_15_INTR_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_SNK_PWR_15_INTR_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_cc_ent_snk_pwr_30_intr_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_INTR_EN_2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_SNK_PWR_30_INTR_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_SNK_PWR_30_INTR_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_cc_ent_snk_pwr_redetect_intr_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_INTR_EN_2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_SNK_PWR_REDETECT_INTR_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_CC_ENT_SNK_PWR_REDETECT_INTR_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_phy_rg_cc1_rpde(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_PHY_RG_CC1_RESISTENCE_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_PHY_RG_CC1_RPDE_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_PHY_RG_CC1_RPDE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_phy_rg_cc1_rp15(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_PHY_RG_CC1_RESISTENCE_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_PHY_RG_CC1_RP15_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_PHY_RG_CC1_RP15_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_phy_rg_cc1_rp3(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_PHY_RG_CC1_RESISTENCE_1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_PHY_RG_CC1_RP3_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_PHY_RG_CC1_RP3_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_phy_rg_cc1_rd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_PHY_RG_CC1_RESISTENCE_1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_PHY_RG_CC1_RD_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_PHY_RG_CC1_RD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_phy_rg_cc2_rpde(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_PHY_RG_CC2_RESISTENCE_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_PHY_RG_CC2_RPDE_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_PHY_RG_CC2_RPDE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_phy_rg_cc2_rp15(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_PHY_RG_CC2_RESISTENCE_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_PHY_RG_CC2_RP15_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_PHY_RG_CC2_RP15_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_phy_rg_cc2_rp3(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_PHY_RG_CC2_RESISTENCE_1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_PHY_RG_CC2_RP3_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_PHY_RG_CC2_RP3_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_phy_rg_cc2_rd(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_PHY_RG_CC2_RESISTENCE_1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_PHY_RG_CC2_RD_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_PHY_RG_CC2_RD_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_sw_force_mode_en_da_cc_rcc1(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SW_FORCE_MODE_ENABLE_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_EN_DA_CC_RCC1_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_EN_DA_CC_RCC1_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_sw_force_mode_en_da_cc_rcc2(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SW_FORCE_MODE_ENABLE_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_EN_DA_CC_RCC2_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_EN_DA_CC_RCC2_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_sw_force_mode_en_da_cc_lev_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SW_FORCE_MODE_ENABLE_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_EN_DA_CC_LEV_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_EN_DA_CC_LEV_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_sw_force_mode_en_da_cc_sw_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SW_FORCE_MODE_ENABLE_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_EN_DA_CC_SW_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_EN_DA_CC_SW_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_sw_force_mode_en_da_cc_bias_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SW_FORCE_MODE_ENABLE_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_EN_DA_CC_BIAS_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_EN_DA_CC_BIAS_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_sw_force_mode_en_da_cc_lpf_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SW_FORCE_MODE_ENABLE_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_EN_DA_CC_LPF_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_EN_DA_CC_LPF_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_sw_force_mode_en_da_cc_adcsw_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SW_FORCE_MODE_ENABLE_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_EN_DA_CC_ADCSW_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_EN_DA_CC_ADCSW_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_sw_force_mode_en_da_cc_sasw_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SW_FORCE_MODE_ENABLE_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_EN_DA_CC_SASW_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_EN_DA_CC_SASW_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_sw_force_mode_en_da_cc_dac_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SW_FORCE_MODE_ENABLE_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_EN_DA_CC_DAC_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_EN_DA_CC_DAC_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_sw_force_mode_en_da_cc_saclk(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SW_FORCE_MODE_ENABLE_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_EN_DA_CC_SACLK_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_EN_DA_CC_SACLK_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_sw_force_mode_en_da_cc_dac_in(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SW_FORCE_MODE_ENABLE_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_EN_DA_CC_DAC_IN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_EN_DA_CC_DAC_IN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_sw_force_mode_da_cc_rpcc1_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SW_FORCE_MODE_VAL_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_DA_CC_RPCC1_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_DA_CC_RPCC1_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_sw_force_mode_da_cc_rdcc1_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SW_FORCE_MODE_VAL_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_DA_CC_RDCC1_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_DA_CC_RDCC1_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_sw_force_mode_da_cc_racc1_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SW_FORCE_MODE_VAL_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_DA_CC_RACC1_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_DA_CC_RACC1_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_sw_force_mode_da_cc_rpcc2_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SW_FORCE_MODE_VAL_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_DA_CC_RPCC2_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_DA_CC_RPCC2_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_sw_force_mode_da_cc_rdcc2_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SW_FORCE_MODE_VAL_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_DA_CC_RDCC2_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_DA_CC_RDCC2_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_sw_force_mode_da_cc_racc2_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SW_FORCE_MODE_VAL_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_DA_CC_RACC2_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_DA_CC_RACC2_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_sw_force_mode_da_cc_lev_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SW_FORCE_MODE_VAL_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_DA_CC_LEV_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_DA_CC_LEV_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_sw_force_mode_da_cc_sw_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SW_FORCE_MODE_VAL_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_DA_CC_SW_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_DA_CC_SW_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_sw_force_mode_da_cc_bias_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SW_FORCE_MODE_VAL_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_DA_CC_BIAS_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_DA_CC_BIAS_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_sw_force_mode_da_cc_lpf_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SW_FORCE_MODE_VAL_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_DA_CC_LPF_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_DA_CC_LPF_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_sw_force_mode_da_cc_adcsw_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SW_FORCE_MODE_VAL_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_DA_CC_ADCSW_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_DA_CC_ADCSW_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_sw_force_mode_da_cc_sasw_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SW_FORCE_MODE_VAL_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_DA_CC_SASW_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_DA_CC_SASW_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_sw_force_mode_da_cc_dac_en(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SW_FORCE_MODE_VAL_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_DA_CC_DAC_EN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_DA_CC_DAC_EN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_sw_force_mode_da_cc_saclk(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SW_FORCE_MODE_VAL_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_DA_CC_SACLK_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_DA_CC_SACLK_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_sw_force_mode_da_cc_dac_in(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SW_FORCE_MODE_VAL_1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_DA_CC_DAC_IN_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_DA_CC_DAC_IN_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_sw_force_mode_en_da_cc_dac_cal(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SW_FORCE_MODE_ENABLE_1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_EN_DA_CC_DAC_CAL_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_EN_DA_CC_DAC_CAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_sw_force_mode_en_da_cc_dac_gain_cal(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SW_FORCE_MODE_ENABLE_1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_EN_DA_CC_DAC_GAIN_CAL_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_EN_DA_CC_DAC_GAIN_CAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_sw_force_mode_da_cc_dac_cal(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SW_FORCE_MODE_VAL_2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_DA_CC_DAC_CAL_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_DA_CC_DAC_CAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_sw_force_mode_da_cc_dac_gain_cal(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_SW_FORCE_MODE_VAL_2),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_DA_CC_DAC_GAIN_CAL_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_FORCE_MODE_DA_CC_DAC_GAIN_CAL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_type_c_dac_cal_start(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_DAC_CALI_CTRL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_TYPE_C_DAC_CAL_START_MASK),
                             (kal_uint32)(MT6392_PMIC_TYPE_C_DAC_CAL_START_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_dac_cal_stage(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_CC_DAC_CALI_CTRL),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_DAC_CAL_STAGE_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_DAC_CAL_STAGE_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_dbg_port_sel_0(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_DEBUG_PORT_SELECT_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_DBG_PORT_SEL_0_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_DBG_PORT_SEL_0_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_dbg_port_sel_1(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_DEBUG_PORT_SELECT_1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_DBG_PORT_SEL_1_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_DBG_PORT_SEL_1_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_dbg_mod_sel(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_DEBUG_MODE_SELECT),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_DBG_MOD_SEL_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_DBG_MOD_SEL_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_sw_dbg_port_0(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_SW_DEBUG_PORT_0),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_DBG_PORT_0_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_DBG_PORT_0_SHIFT)
	                         );
  pmic_unlock();
}

void mt6392_upmu_set_reg_type_c_sw_dbg_port_1(kal_uint32 val)
{
  kal_uint32 ret=0;

  pmic_lock();
  ret=pmic_config_interface( (kal_uint32)(MT6392_TYPE_C_SW_DEBUG_PORT_1),
                             (kal_uint32)(val),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_DBG_PORT_1_MASK),
                             (kal_uint32)(MT6392_PMIC_REG_TYPE_C_SW_DBG_PORT_1_SHIFT)
	                         );
  pmic_unlock();
}

