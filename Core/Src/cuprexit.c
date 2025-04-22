#include <stdint.h>
#include <stdio.h>
#include "spi.h"
#include "cuprexit.h"
#include "usbd_cdc_if.h"


void sendCommand(CUPREXIT_Device *device, CUPREXIT_Command command, uint8_t *data, uint8_t size) {
    selectDevice(device);
    sendSPICommand(device, command, data, size);
}

uint8_t isActive(CUPREXIT_Device *device) {
    selectDevice(device);
    sendSPICommand(device, CUPREXIT_COMMAND_GET_STATUS, NULL, 0);
    uint16_t Cycle = 40;
    while (spi_response == 0){
        HAL_Delay(50);
        if (spi_response == 1)
            return 1;
        else
            Cycle--;
        
        if (Cycle == 0) 
            return 0;
    }
    return 0;
}


// Main function to handle commands
void handleCUPCommand(uint8_t command, CUPREXIT_Device *device) {
    switch (command) {
        case CUPREXIT_COMMAND_GET_CALIB:
            getAllCuCalib(device);
            break;
        case CUPREXIT_COMMAND_SET_CALIB:
            setCuCalib(device);
            break;
        case CUPREXIT_COMMAND_GET_MEAS:
            getMeas(device);
            break;
        case CUPREXIT_COMMAND_GET_STATUS:
            getStatus(device);
            break;
        case CUPREXIT_COMMAND_MEAS:
            meas();
            break;
        case CUPREXIT_COMMAND_SET_USER_ID:
            //setUserId();
            break;
        case CUPREXIT_COMMAND_GET_USER_ID:
            getUserId(device);
            break;
        case CUPREXIT_COMMAND_GET_UID:
            getUid(device);
            break;
        case CUPREXIT_COMMAND_RESET:
            resetCu(device);
            break;
        case CUPREXIT_COMMAND_PING:
            // Add ping functionality here
            break;
    }
}

// Function to initialize CUPREXIT_Device structure
void CUPREXIT_InitDevice(CUPREXIT_Device *device, SPI_HandleTypeDef *hspi, uint8_t device_id, uint16_t pin, GPIO_TypeDef* port) {
    device->hspi = hspi;
    device->User_ID = device_id;
    device->pin = pin;
    device->port = port;
    device->active = 0;
    handleCUPCommand(CUPREXIT_COMMAND_GET_UID, device);
    handleCUPCommand(CUPREXIT_COMMAND_GET_CALIB, device);
}


void checkCuprexitDevices(CUPREXIT_Device CU_devices[]) {
    for (int i = 0; i < NMBR_CU; i++) {
        isActive(&CU_devices[i]);
        checkUID(&CU_devices[i]);
    }
}

void getCuCalib(CUPREXIT_Device *device) {
    spi_response = 0;  // připravíme flag
    sendSPICommand(device, CUPREXIT_COMMAND_GET_CALIB, NULL, 0);
    while (spi_response == 0);  // čekáme na odpověď
    if (SPIRxBuffer[2] == CUx_CAL) {
        device->active = 0;
    } else {
        // Error handling
    }   
}

void getAllCuCalib(CUPREXIT_Device CU_devices[]) {
    uint8_t descriptor = CUx_CAL;

    addDataToUSBBuffer(NULL, 1, descriptor);

    // Iterace přes všech 10 CUPREXIT zařízení
    for (int i = 0; i < NMBR_CU; i++) {
        CUPREXIT_Device *dev = &CU_devices[i];

        // Pošleme příkaz na získání kalibrace
        getCuCalib(dev);

        // Přidáme ID zařízení (User_ID – 1 bajt)
        addDataToUSBBuffer(&(dev->User_ID), 1, 0);

        // Přidáme 40 bajtů (10 floatů)
        addDataToUSBBuffer((uint8_t *)SPIRxBuffer + 2, 40, 0);  // Přeskočíme [0]=status, [1]=descriptor
    }
}


void setCuCalib(CUPREXIT_Device *device) {
    spi_response = 0;
    sendSPICommand(device, CUPREXIT_COMMAND_SET_CALIB, (uint8_t *)device->calib, 42);
    // Čekání na potvrzení přenosu
    while (spi_response == 0);
    if (SPIRxBuffer[2] == 1) {
        device->active = 0;
    } else {
        // Error handling
    }
}
void getMeas(CUPREXIT_Device *device) {
    selectDevice(device);
    sendSPICommand(device, CUPREXIT_COMMAND_MEAS, NULL, 0);
    while (spi_response == 0);  // čekáme na odpověď
    if (SPIRxBuffer[2] == CUx_TEMP) {
        device->active = 0;
    } else {
        // Error handling
    }
    deselectDevice(device);
}
void getAllMeasAndSend(CUPREXIT_Device CU_devices[], uint8_t *brain_uid) {
    uint8_t descriptor = CUx_TEMP;

    addDataToUSBBuffer(&descriptor, 1, 0);

    // Iterace přes všech 10 CUPREXIT zařízení
    for (int i = 0; i < NMBR_CU; i++) {
        CUPREXIT_Device *dev = &CU_devices[i];
        while (getStatus(dev) == CUPREXIT_STATUS_MEASUREMENT) {
            HAL_Delay(100);
        }  
        // Pošleme příkaz na získání kalibrace
        getMeas(dev);

        // Přidáme ID zařízení (User_ID – 1 bajt)
        addDataToUSBBuffer(&(dev->User_ID), 1, 0);

        // Přidáme 40 bajtů (10 floatů)
        addDataToUSBBuffer((uint8_t *)SPIRxBuffer + 2, 40, 0);  // Přeskočíme [0]=status, [1]=descriptor
        
    }
}
uint8_t getStatus(CUPREXIT_Device *device) {
    spi_response = 0;
    sendSPICommand(device, CUPREXIT_COMMAND_GET_STATUS, NULL, 0);
    while (spi_response == 0);
    deselectDevice(device);
    return SPIRxBuffer[2];
}
void meas() {
    selectAllDevices(CU_devices);
    sendSPICommand(NULL, CUPREXIT_COMMAND_MEAS, NULL, 0);
    while (HAL_SPI_GetState(&hspi3) != HAL_SPI_STATE_READY);
    deselectAllDevices(CU_devices);
}

void setUserId(CUPREXIT_Device *device, uint8_t user_id) {
    spi_response = 0;
    selectDevice(device);
    sendSPICommand(device, CUPREXIT_COMMAND_GET_USER_ID, &user_id, 1);
    while (spi_response == 0);  // čekáme na odpověď
    if (SPIRxBuffer[2] == 1) {
        device->User_ID = user_id;
        deselectDevice(device); 
    } else{
        // Error handling
    }
}

uint8_t getUserId(CUPREXIT_Device *device) {
    selectDevice(device);
    sendSPICommand(device, CUPREXIT_COMMAND_GET_USER_ID, NULL, 0);
    while (spi_response == 0);  // čekáme na odpověď 
    deselectDevice(device); 
    return SPIRxBuffer[2];

}

void getUid(CUPREXIT_Device *device) {
    selectDevice(device);
    sendSPICommand(device, CUPREXIT_COMMAND_GET_UID, NULL, 0);
    while (spi_response == 0);  // čekáme na odpověď

    device->Device_UID[0] = (SPIRxBuffer[2]<<24) + (SPIRxBuffer[3]<<16) + (SPIRxBuffer[4]<<8) + SPIRxBuffer[5];
    device->Device_UID[1] = (SPIRxBuffer[6]<<24) + (SPIRxBuffer[7]<<16) + (SPIRxBuffer[8]<<8) + SPIRxBuffer[9];
    device->Device_UID[2] = (SPIRxBuffer[10]<<24) + (SPIRxBuffer[11]<<16) + (SPIRxBuffer[12]<<8) + SPIRxBuffer[13];
    deselectDevice(device);     
}

void resetCu(CUPREXIT_Device *device) {
    spi_response = 0;
    sendSPICommand(device, CUPREXIT_COMMAND_RESET, NULL, 0);
    while (spi_response == 0);

    if (SPIRxBuffer[2] == 1) {
        device->active = 0;
        deselectDevice(device); 
    }
}
