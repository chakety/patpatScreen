DIR_Config   = ./lib/Config
DIR_Driver	 = ./lib/Driver
DIR_GUI		 = ./lib/GUI
DIR_FONTS    = ./lib/Fonts
DIR_EPD		 = ./lib/EPD
DIR_func     = ./func
DIR_BIN      = ./bin

OBJ_C = $(wildcard ${DIR_Driver}/*.c ${DIR_GUI}/*.c ${DIR_EPD}/*.c ${DIR_Config}/*.c ${DIR_FONTS}/*.c ${DIR_func}/*.c main.c)
OBJ_O = $(patsubst %.c,${DIR_BIN}/%.o,$(notdir ${OBJ_C}))

TARGET = main

# USELIB = USE_BCM2835_LIB
# USELIB = USE_WIRINGPI_LIB
USELIB = USE_LGPIO_LIB
# USELIB = USE_GPIOD_LIB

DEBUG = -D $(USELIB)

ifeq ($(USELIB), USE_BCM2835_LIB)
    LIB = -lbcm2835 -lm 
	OBJ_O := $(filter-out ${DIR_BIN}/RPI_gpiod.o ${DIR_BIN}/dev_hardware_SPI.o ${DIR_BIN}/dev_hardware_i2c.o, ${OBJ_O})
else ifeq ($(USELIB), USE_WIRINGPI_LIB)
    LIB = -lwiringPi -lm 
	OBJ_O := $(filter-out ${DIR_BIN}/RPI_gpiod.o ${DIR_BIN}/dev_hardware_SPI.o ${DIR_BIN}/dev_hardware_i2c.o, ${OBJ_O})
else ifeq ($(USELIB), USE_LGPIO_LIB)
	LIB += -llgpio -lm 
	OBJ_O := $(filter-out ${DIR_BIN}/RPI_gpiod.o ${DIR_BIN}/dev_hardware_SPI.o ${DIR_BIN}/dev_hardware_i2c.o, ${OBJ_O})
else ifeq ($(USELIB), USE_GPIOD_LIB)
    LIB = -lgpiod -lm
endif
LIB += -lpthread -lcurl -L/home/chakety/Downloads/cJSON/build -lcjson
CFLAGS += -I/usr/include/curl -I/home/chakety/Downloads/cJSON/include
CFLAGS += -I/usr/include/curl
CC = gcc
MSG = -g -O0 -Wall
CFLAGS += $(MSG) $(DEBUG)

${TARGET}:${OBJ_O}
	$(CC) $(CFLAGS) $(OBJ_O) -o $@ $(LIB)
    
$(shell mkdir -p $(DIR_BIN))

${DIR_BIN}/main.o: main.c
	$(CC) $(CFLAGS) -c  $< -o $@ -I $(DIR_Config) -I $(DIR_Driver)  -I $(DIR_EPD) -I $(DIR_GUI) -I $(DIR_func)
    
${DIR_BIN}/%.o:$(DIR_Driver)/%.c
	$(CC) $(CFLAGS) -c  $< -o $@ -I $(DIR_Config) $(DEBUG)

${DIR_BIN}/%.o:$(DIR_EPD)/%.c
	$(CC) $(CFLAGS) -c  $< -o $@ -I $(DIR_Config) $(DEBUG)
    
${DIR_BIN}/%.o:$(DIR_FONTS)/%.c 
	$(CC) $(CFLAGS) -c  $< -o $@ $(DEBUG)
	
${DIR_BIN}/%.o:$(DIR_GUI)/%.c
	$(CC) $(CFLAGS) -c  $< -o $@ -I $(DIR_Config) $(DEBUG)
	
${DIR_BIN}/%.o:$(DIR_Config)/%.c
	$(CC) $(CFLAGS) -c  $< -o $@ $(LIB)

${DIR_BIN}/%.o:$(DIR_func)/%.c
	$(CC) $(CFLAGS) -c  $< -o $@ -I $(DIR_Config) $(DEBUG)
	
clean :
	rm $(DIR_BIN)/*.* 
	rm $(TARGET) 