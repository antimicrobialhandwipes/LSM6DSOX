#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

// PICO 2 Pinout: https://pip-assets.raspberrypi.com/categories/1005-raspberry-pi-pico-2/documents/RP-008301-DS-1-Pico-2-Pinout.pdf

/*
    Selected IMU: LSM6DSOX
    Datasheet: https://www.st.com/resource/en/datasheet/lsm6dsox.pdf
*/

#define MISO 16 // RX
#define CSn 17
#define SCK 18
#define MOSI 19 // TX
#define SPI_CORE spi0
#define SPI_CLK_FREQ 10000000

void spi_core_init(spi_inst_t *spi_inst, uint8_t miso, uint8_t mosi, uint8_t sck, uint8_t cs){
    spi_init(spi_inst, SPI_CLK_FREQ);

    // Utilizes the Pi Pico 2's SPI Functionality rather than its GPIO Functionality
    gpio_set_function(miso, GPIO_FUNC_SPI);
    gpio_set_function(mosi, GPIO_FUNC_SPI);
    gpio_set_function(sck, GPIO_FUNC_SPI);
    // gpio_set_function(cs, GPIO_FUNC_SPI); // --> Possible Redundant (commented out later)

    // Register Mappings (P. 43), Timing Diagrams (P. 13), and BUS Interface (P. 19)
    // Stated that the Set Function is set to Mode 3, but in actuality, it's Mode 1 (Double-Check)
    spi_set_format(spi_inst, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);

    gpio_init(cs);
    gpio_set_dir(cs, GPIO_OUT);

    /*
        Ref -> Timing Diagram (P. 13): When the SPI Interface is idle, Chip Select is High
    */
    gpio_put(cs, 1);
}

int reg_read(spi_inst_t *spi_inst, const uint8_t cs, uint8_t reg, uint8_t *buf, const uint8_t nbytes){
    int num_bytes_read = 0;

    if (nbytes < 1){
        return -1;
    }

    /* 
        Set bit 7 (the MSB) of the address byte to 1
        The IMU reads this bit as the R/W flag: 1 = read, 0 = write

        Ref -> BUS Interface (P. 19)
    */
    reg = 0x80 | reg;

    gpio_put(cs, 0);
    spi_write_blocking(spi_inst, &reg, 1);
    num_bytes_read = spi_read_blocking(spi_inst, 0, buf, nbytes);
    gpio_put(cs, 1);

    return num_bytes_read;
}

int main(){
    stdio_init_all();

    while(1){

    }
    return 0;
}