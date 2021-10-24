#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([i2c_spi_chips], [0.0], [trevor.gowen@blueyonder.co.uk])
AC_CONFIG_SRCDIR([common/rcs_scm.h])
# Platform & System ...
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AM_INIT_AUTOMAKE([-Wall foreign]) 
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_PROG_RANLIB
AC_CHECK_LIB([wiringPi], [wiringPiI2CSetup])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h strings.h unistd.h])

# Check for wiringPi.h ...
have_wiringpi_h=no
AC_CHECK_HEADERS([wiringPi.h], [have_wiringpi_h=yes])

if test "x${have_wiringpi_h}" = xno; then
   AC_MSG_WARN([
   **************************************
   Unable to find wiringPi header ...
   Some program features may be disabled!
   **************************************])
fi

if test "x${have_wiringpi_h}" = xyes; then
   
   AC_MSG_NOTICE([
   ************************************
   wiringPi header available ... 
   (HAVE_WIRINGPI_H defined in config.h)
   Full functionality available!
   ************************************])
fi

# Check for wiringPiI2C.h ...
have_wiringpii2c_h=no
AC_CHECK_HEADERS([wiringPiI2C.h], [have_wiringpii2c_h=yes])

if test "x${have_wiringpii2c_h}" = xno; then
   AC_MSG_WARN([
   **************************************
   Unable to find wiringPiI2C header ...
   Some program features may be disabled!
   **************************************])
fi

if test "x${have_wiringpii2c_h}" = xyes; then
   
   AC_MSG_NOTICE([
   ************************************
   wiringPiI2C header available ... 
   (HAVE_WIRINGPII2C_H defined in config.h)
   Full functionality available!
   ************************************])
fi

# Check for wiringPiSPI.h ...
have_wiringpispi_h=no
AC_CHECK_HEADERS([wiringPiSPI.h], [have_wiringpispi_h=yes])

if test "x${have_wiringpispi_h}" = xno; then
   AC_MSG_WARN([
   **************************************
   Unable to find wiringPiSPI header ...
   Some program features may be disabled!
   **************************************])
fi

if test "x${have_wiringpispi_h}" = xyes; then
   
   AC_MSG_NOTICE([
   ************************************
   wiringPiSPI header available ... 
   (HAVE_WIRINGPISPI_H defined in config.h)
   Full functionality available!
   ************************************])
fi

# Check for pigpio.h ...
have_pigpio_h=no
AC_CHECK_HEADERS([pigpio.h], [have_pigpio_h=yes])

if test "x${have_pigpio_h}" = xno; then
   AC_MSG_WARN([
   **************************************
   Unable to find pigpio header ...
   Some program features may be disabled!
   **************************************])
fi

if test "x${have_pigpio_h}" = xyes; then

   AC_SEARCH_LIBS([gpioInitialise], [pigpio], , , [-lpthread -lrt])
# Libraries needed to use pigpio etc. ...
   AC_SEARCH_LIBS([pthread_create], [pthread])
   AC_SEARCH_LIBS([clock_gettime], [rt])
   
   AC_MSG_NOTICE([
   ************************************
   pigpio header available ... 
   (HAVE_PIGPIO_H defined in config.h)
   Full functionality available!
   ************************************])
fi

# Check for pigpiod_if2.h ...
have_pigpiod_if2_h=no
AC_CHECK_HEADERS([pigpiod_if2.h], [have_pigpiod_if2_h=yes])

if test "x${have_pigpiod_if2_h}" = xno; then
   AC_MSG_WARN([
   **************************************
   Unable to find pigpiod_if2 header ...
   Some program features may be disabled!
   **************************************])
fi

if test "x${have_pigpiod_if2_h}" = xyes; then

   AC_SEARCH_LIBS([pigpio_start], [pigpiod_if2], , , [-lpthread -lrt])
# Libraries needed to use pigpiod_if2 etc. ...
   AC_SEARCH_LIBS([pthread_create], [pthread])
   AC_SEARCH_LIBS([clock_gettime], [rt])
   
   AC_MSG_NOTICE([
   ************************************
   pigpiod_if2 header available ... 
   (HAVE_PIGPIOD_IF2_H defined in config.h)
   Full functionality available!
   ************************************])
fi

## Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([gethostname strcasecmp strtoul])

AC_CONFIG_FILES([Makefile
                 common/Makefile
                 src/Makefile])
AC_OUTPUT
