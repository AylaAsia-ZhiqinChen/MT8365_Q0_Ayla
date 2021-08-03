# In case the platform does NOT support this type of sensors
ccflags-y += -I$(srctree)/drivers/misc/mediatek/sensors-1.0/hwmon/include
ccflags-y += -I$(srctree)/drivers/misc/mediatek/sensors-1.0/include

obj-y   +=  accel.o accel_factory.o

obj-$(CONFIG_MTK_ACCELHUB) += accelhub/
obj-$(CONFIG_MTK_BMI160_I2C)   += bmi160-i2c/
obj-$(CONFIG_MTK_LIS3DH_NEW) += lis3dh/
obj-$(CONFIG_MTK_MC3410_I2C) += mc3410-i2c/
obj-$(CONFIG_MTK_MC3433) += mc3433/
obj-$(CONFIG_MTK_MPU6050G) += mpu6050g/
obj-$(CONFIG_MTK_STK8BAXX) += stk8baxx/
obj-$(CONFIG_MTK_BMI160ACC_YT)   += bmi160acc_yt/
obj-$(CONFIG_MTK_BMI160STC_YT)   += bmi160stc_yt/
obj-$(CONFIG_MTK_ICM426XX_ACC)   += icm426xx_acc/