## Process this file with automake to produce Makefile.in
bin_PROGRAMS = demo dual_lcd_i2c4-8bit_demo lcd_demo lcd_fi2c_demo lcd_hi2c_demo \
lcd_mi2c_demo lcd_ibo_i2c_demo lcd_gpio4b_demo lcd_spi4b_demo lcd_spi4bV2_demo \
lcd_spi8b_demo lcd_structures_list glcd_spi8b_demo spi_ledx_demo spi_glcd_demo \
spi_glcdV2_demo spi_glcd_cacc_demo oled_i2c_demo glcd_font_tool spi_led_4dq_demo \
spi_led_4drrg_demo i2c_rgb_led_demo glcd_serial_demo oled_spi_demo i2c_ledx_demo \
i2c_mxledx_demo i2c_clkled_demo lcd_gpio4bV2_demo spi_glcdHP3_demo spi_glcdV2N_demo \
glcd_spi8bA_demo spi_led_2x4d_demo oled_spiV2_demo
#bbx_spi_glcd_demo
demo_SOURCES = ../common/rcs_scm.h demo.c
demo_CPPFLAGS = -I$(top_srcdir)/common
demo_LDADD = ../common/libparsel.a ../common/libi2c_chips.a \
../common/libi2c_wraps.a ../common/libled_disptools.a ../common/liblcd_disptools.a
dual_lcd_i2c4_8bit_demo_SOURCES = ../common/rcs_scm.h dual_lcd_i2c4-8bit_demo.c
dual_lcd_i2c4_8bit_demo_CPPFLAGS = -I$(top_srcdir)/common
dual_lcd_i2c4_8bit_demo_LDADD = ../common/libparsel.a ../common/libi2c_chips.a \
../common/libi2c_wraps.a ../common/liblcd_disptools.a
lcd_demo_SOURCES = ../common/rcs_scm.h lcd_demo.c
lcd_demo_CPPFLAGS = -I$(top_srcdir)/common
lcd_demo_LDADD = ../common/libparsel.a ../common/libi2c_chips.a \
../common/libi2c_wraps.a ../common/liblcd_disptools.a
lcd_fi2c_demo_SOURCES = ../common/rcs_scm.h lcd_fi2c_demo.c
lcd_fi2c_demo_CPPFLAGS = -I$(top_srcdir)/common
lcd_fi2c_demo_LDADD = ../common/libparsel.a ../common/libi2c_chips.a \
../common/libi2c_wraps.a ../common/liblcd_disptools.a
lcd_hi2c_demo_SOURCES = ../common/rcs_scm.h lcd_hi2c_demo.c
lcd_hi2c_demo_CPPFLAGS = -I$(top_srcdir)/common
lcd_hi2c_demo_LDADD = ../common/libparsel.a ../common/libi2c_chips.a \
../common/libi2c_wraps.a ../common/liblcd_disptools.a ../common/libpigpio_wraps.a
lcd_mi2c_demo_SOURCES = ../common/rcs_scm.h lcd_mi2c_demo.c
lcd_mi2c_demo_CPPFLAGS = -I$(top_srcdir)/common
lcd_mi2c_demo_LDADD = ../common/libparsel.a ../common/libi2c_chips.a \
../common/libi2c_wraps.a ../common/liblcd_disptools.a
lcd_ibo_i2c_demo_SOURCES = ../common/rcs_scm.h lcd_ibo_i2c_demo.c
lcd_ibo_i2c_demo_CPPFLAGS = -I$(top_srcdir)/common
lcd_ibo_i2c_demo_LDADD = ../common/libparsel.a ../common/libi2c_chips.a \
../common/libi2c_wraps.a ../common/liblcd_disptools.a
lcd_gpio4b_demo_SOURCES = ../common/rcs_scm.h lcd_gpio4b_demo.c
lcd_gpio4b_demo_CPPFLAGS = -I$(top_srcdir)/common
lcd_gpio4b_demo_LDADD = ../common/libparsel.a ../common/liblcd_disptools.a \
../common/libi2c_wraps.a
lcd_spi4b_demo_SOURCES = ../common/rcs_scm.h lcd_spi4b_demo.c
lcd_spi4b_demo_CPPFLAGS = -I$(top_srcdir)/common
lcd_spi4b_demo_LDADD = ../common/libparsel.a ../common/liblcd_disptools.a \
../common/libi2c_wraps.a ../common/libspi_wraps.a
lcd_spi4bV2_demo_SOURCES = ../common/rcs_scm.h lcd_spi4bV2_demo.c
lcd_spi4bV2_demo_CPPFLAGS = -I$(top_srcdir)/common
lcd_spi4bV2_demo_LDADD = ../common/libparsel.a ../common/liblcd_disptools.a \
../common/libi2c_wraps.a ../common/libspi_wraps.a
lcd_spi8b_demo_SOURCES = ../common/rcs_scm.h lcd_spi8b_demo.c
lcd_spi8b_demo_CPPFLAGS = -I$(top_srcdir)/common
lcd_spi8b_demo_LDADD = ../common/libparsel.a ../common/liblcd_disptools.a \
../common/libi2c_wraps.a ../common/libspi_wraps.a
lcd_structures_list_SOURCES = ../common/rcs_scm.h lcd_structures_list.c
lcd_structures_list_CPPFLAGS = -I$(top_srcdir)/common
lcd_structures_list_LDADD = ../common/libparsel.a ../common/liblcd_disptools.a \
../common/libi2c_wraps.a ../common/libspi_wraps.a
glcd_spi8b_demo_SOURCES = ../common/rcs_scm.h glcd_spi8b_demo.c
glcd_spi8b_demo_CPPFLAGS = -I$(top_srcdir)/common
glcd_spi8b_demo_LDADD = ../common/libparsel.a ../common/liblcd_disptools.a \
../common/libglcd_xbmtools.a ../common/libi2c_wraps.a ../common/libspi_wraps.a
spi_ledx_demo_SOURCES = ../common/rcs_scm.h ../common/font.h  \
../common/PCD8544_font.h spi_ledx_demo.c
spi_ledx_demo_CPPFLAGS = -I$(top_srcdir)/common
spi_ledx_demo_LDADD = ../common/libparsel.a ../common/libspi_chips.a \
../common/libspi_wraps.a
spi_glcd_demo_SOURCES = ../common/rcs_scm.h ../common/PCD8544_font.h spi_glcd_demo.c
spi_glcd_demo_CPPFLAGS = -I$(top_srcdir)/common
spi_glcd_demo_LDADD = ../common/libparsel.a ../common/libglcd_xbmtools.a \
../common/libspi_chips.a ../common/libspi_wraps.a ../common/libi2c_wraps.a \
../common/libspi_PCD8544.a
spi_glcdV2_demo_SOURCES = ../common/rcs_scm.h ../common/PCD8544_font.h spi_glcdV2_demo.c
spi_glcdV2_demo_CPPFLAGS = -I$(top_srcdir)/common
spi_glcdV2_demo_LDADD = ../common/libparsel.a ../common/libglcd_xbmtools.a \
../common/libspi_chips.a ../common/libspi_wraps.a ../common/libi2c_wraps.a \
../common/libspi_PCD8544.a
spi_glcd_cacc_demo_SOURCES = ../common/rcs_scm.h ../common/PCD8544_font.h \
spi_glcd_cacc_demo.c
spi_glcd_cacc_demo_CPPFLAGS = -I$(top_srcdir)/common
spi_glcd_cacc_demo_LDADD = ../common/libparsel.a ../common/libglcd_xbmtools.a \
../common/libspi_chips.a ../common/libspi_wraps.a ../common/libi2c_wraps.a \
../common/libspi_PCD8544.a
oled_i2c_demo_SOURCES = ../common/rcs_scm.h ../common/oled128.h ../common/oled256.h \
../common/jace128.h oled_i2c_demo.c
oled_i2c_demo_CPPFLAGS = -I$(top_srcdir)/common
oled_i2c_demo_LDADD = ../common/libparsel.a ../common/libi2c_chips.a \
../common/libi2c_wraps.a ../common/libpigpio_wraps.a
glcd_font_tool_SOURCES = ../common/rcs_scm.h ../common/font.h glcd_font_tool.c
glcd_font_tool_CPPFLAGS = -I$(top_srcdir)/common
glcd_font_tool_LDADD = ../common/libparsel.a ../common/libglcd_fonttools.a
spi_led_4dq_demo_SOURCES = ../common/rcs_scm.h spi_led_4dq_demo.c
spi_led_4dq_demo_CPPFLAGS = -I$(top_srcdir)/common
spi_led_4dq_demo_LDADD = ../common/libparsel.a ../common/libspi_chips.a \
../common/libspi_wraps.a ../common/libled_disptools.a
spi_led_4drrg_demo_SOURCES = ../common/rcs_scm.h spi_led_4drrg_demo.c
spi_led_4drrg_demo_CPPFLAGS = -I$(top_srcdir)/common
spi_led_4drrg_demo_LDADD = ../common/libparsel.a ../common/libspi_chips.a \
../common/libspi_wraps.a ../common/libled_disptools.a
i2c_rgb_led_demo_SOURCES = ../common/rcs_scm.h i2c_rgb_led_demo.c
i2c_rgb_led_demo_CPPFLAGS = -I$(top_srcdir)/common
i2c_rgb_led_demo_LDADD = ../common/libparsel.a ../common/libi2c_chips.a \
../common/libi2c_wraps.a
glcd_serial_demo_SOURCES = ../common/rcs_scm.h glcd_serial_demo.c
glcd_serial_demo_CPPFLAGS = -I$(top_srcdir)/common
glcd_serial_demo_LDADD = ../common/libparsel.a ../common/liblcd_disptools.a \
../common/libglcd_xbmtools.a ../common/libi2c_wraps.a ../common/libspi_wraps.a
oled_spi_demo_SOURCES = ../common/rcs_scm.h ../common/oled128.h ../common/oled256.h \
../common/jace128.h oled_spi_demo.c
oled_spi_demo_CPPFLAGS = -I$(top_srcdir)/common
oled_spi_demo_LDADD = ../common/libparsel.a ../common/libspi_chips.a \
../common/libspi_wraps.a  ../common/libi2c_wraps.a
i2c_ledx_demo_SOURCES = ../common/rcs_scm.h ../common/font.h  \
../common/jace128.h ../common/PCD8544_font.h i2c_ledx_demo.c
i2c_ledx_demo_CPPFLAGS = -I$(top_srcdir)/common
i2c_ledx_demo_LDADD = ../common/libparsel.a ../common/libi2c_chips.a \
../common/libi2c_wraps.a ../common/libpigpiod_wraps.a
i2c_mxledx_demo_SOURCES = ../common/rcs_scm.h i2c_mxledx_demo.c
i2c_mxledx_demo_CPPFLAGS = -I$(top_srcdir)/common
i2c_mxledx_demo_LDADD = ../common/libparsel.a ../common/libi2c_chips.a \
../common/libi2c_wraps.a ../common/libled_disptools.a
i2c_clkled_demo_SOURCES = ../common/rcs_scm.h i2c_clkled_demo.c
i2c_clkled_demo_CPPFLAGS = -I$(top_srcdir)/common
i2c_clkled_demo_LDADD = ../common/libparsel.a ../common/libi2c_chips.a \
../common/libi2c_wraps.a ../common/libled_disptools.a ../common/libpigpio_wraps.a \
../common/libpigpiod_wraps.a
lcd_gpio4bV2_demo_SOURCES = ../common/rcs_scm.h lcd_gpio4bV2_demo.c
lcd_gpio4bV2_demo_CPPFLAGS = -I$(top_srcdir)/common
lcd_gpio4bV2_demo_LDADD = ../common/libparsel.a ../common/liblcd_disptools.a \
../common/libi2c_wraps.a
spi_glcdHP3_demo_SOURCES = ../common/rcs_scm.h ../common/PCD8544_font.h spi_glcdHP3_demo.c
spi_glcdHP3_demo_CPPFLAGS = -I$(top_srcdir)/common
spi_glcdHP3_demo_LDADD = ../common/libparsel.a ../common/libglcd_xbmtools.a \
../common/libspi_chips.a ../common/libspi_wraps.a ../common/libi2c_wraps.a \
../common/libspi_PCD8544.a
spi_glcdV2N_demo_SOURCES = ../common/rcs_scm.h ../common/PCD8544_font.h spi_glcdV2N_demo.c
spi_glcdV2N_demo_CPPFLAGS = -I$(top_srcdir)/common
spi_glcdV2N_demo_LDADD = ../common/libparsel.a ../common/libglcd_xbmtools.a \
../common/libspi_chips.a ../common/libspi_wraps.a ../common/libi2c_wraps.a \
../common/libspi_PCD8544.a#bbx_spi_glcd_demo_SOURCES = ../common/rcs_scm.h ../common/PCD8544_font.h bbx_spi_glcd_demo.c
glcd_spi8bA_demo_SOURCES = ../common/rcs_scm.h glcd_spi8bA_demo.c
glcd_spi8bA_demo_CPPFLAGS = -I$(top_srcdir)/common
glcd_spi8bA_demo_LDADD = ../common/libparsel.a ../common/liblcd_disptools.a \
../common/libglcd_xbmtools.a ../common/libi2c_wraps.a ../common/libspi_wraps.a
spi_led_2x4d_demo_SOURCES = ../common/rcs_scm.h spi_led_2x4d_demo.c
spi_led_2x4d_demo_CPPFLAGS = -I$(top_srcdir)/common
spi_led_2x4d_demo_LDADD = ../common/libparsel.a ../common/libspi_chips.a \
../common/libspi_wraps.a ../common/libled_disptools.a
oled_spiV2_demo_SOURCES = ../common/rcs_scm.h ../common/oled128.h ../common/oled256.h \
../common/jace128.h oled_spiV2_demo.c
oled_spiV2_demo_CPPFLAGS = -I$(top_srcdir)/common
oled_spiV2_demo_LDADD = ../common/libparsel.a ../common/libspi_chips.a \
../common/libspi_wraps.a  ../common/libi2c_wraps.a
#bbx_spi_glcd_demo_CPPFLAGS = -I$(top_srcdir)/common
#bbx_spi_glcd_demo_LDADD = ../common/libparsel.a ../common/libspi_chips.a \
#../common/libspi_wraps.a ../common/libi2c_wraps.a ../common/libspi_PCD8544.a \
#/usr/local/lib/libpigpio.a
