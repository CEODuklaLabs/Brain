#ifndef CUPREXIT_H
#define CUPREXIT_H

#include <stdint.h>
#include "stm32l4xx_hal.h" // Adjust this include according to your MCU
#include "main.h"
#include "spi.h"
#include "cuprexit.h"

typedef struct {
    SPI_HandleTypeDef *hspi;
    uint8_t User_ID;
    uint16_t pin;
    uint32_t port;
    uint32_t Device_UID[3];
    uint8_t active;
    float calib[10];
} CUPREXIT_Device;

typedef enum {
    CUPREXIT_COMMAND_GET_CALIB,
    CUPREXIT_COMMAND_SET_CALIB,
    CUPREXIT_COMMAND_GET_MEAS,
    CUPREXIT_COMMAND_GET_STATUS,
    CUPREXIT_COMMAND_MEAS,
    CUPREXIT_COMMAND_SET_USER_ID,
    CUPREXIT_COMMAND_GET_USER_ID,
    CUPREXIT_COMMAND_GET_UID,
    CUPREXIT_COMMAND_RESET,
    CUPREXIT_COMMAND_PING,
    CUPREXIT_COMMAND_SLEEP
} CUPREXIT_Command;

typedef enum {
    CUPREXIT_STATUS_OK,
    CUPREXIT_STATUS_ERROR,
    CUPREXIT_STATUS_MEASUREMENT = 0x03
} CUPREXIT_Status;


extern CUPREXIT_Command active_command;
extern CUPREXIT_Device *active_device;

extern CUPREXIT_Device CU_devices[NMBR_CU];


void CUPREXIT_InitDevice(CUPREXIT_Device *device, SPI_HandleTypeDef *hspi, uint8_t device_id, uint16_t pin, uint16_t port);

// Function prototypes
/**
 * @brief Odešle příkaz do zařízení CUPREXIT.
 * 
 * @param device Ukazatel na zařízení CUPREXIT.
 * @param command Příkaz, který má být odeslán.
 * @param data Ukazatel na data, která mají být odeslána.
 * @param size Velikost dat, která mají být odeslána.
 */
void sendCommand(CUPREXIT_Device *device, CUPREXIT_Command command, uint8_t *data, uint8_t size);

/**
 * @brief Zjistí, zda je zařízení CUPREXIT aktivní.
 * 
 * @param device Ukazatel na zařízení CUPREXIT.
 * @return uint8_t 1 pokud je zařízení aktivní, jinak 0.
 */
uint8_t isActive(CUPREXIT_Device *device);

/**
 * @brief Zpracuje příkaz CUP.
 * 
 * @param command Příkaz, který má být zpracován.
 */
void handleCUPCommand(uint8_t command, CUPREXIT_Device *device);

/**
 * @brief Zkontroluje všechna zařízení CUPREXIT.
 * 
 * @param CU_devices Pole zařízení CUPREXIT.
 */
void checkCuprexitDevices(CUPREXIT_Device CU_devices[]);

/**
 * @brief Zkontroluje UID zařízení CUPREXIT.
 * 
 * @param device Ukazatel na zařízení CUPREXIT.
 */
void checkUID(CUPREXIT_Device *device);

/**
 * @brief Zpracuje odpověď na kontrolu UID.
 */
void checkUIDresponse();

/**
 * @brief Získá kalibraci mědi ze zařízení CUPREXIT.
 * 
 * @param device Ukazatel na zařízení CUPREXIT.
 */
void getCuCalib(CUPREXIT_Device *device);

/**
 * @brief Získá kalibraci mědi ze všech zařízení CUPREXIT.
 * 
 * @param CU_devices Pole zařízení CUPREXIT.
 */
void getAllCuCalib(CUPREXIT_Device CU_devices[]);

/**
 * @brief Nastaví kalibraci mědi pro zařízení CUPREXIT.
 * 
 * @param device Ukazatel na zařízení CUPREXIT.
 */
void setCuCalib(CUPREXIT_Device *device);

/**
 * @brief Získá měření ze zařízení CUPREXIT.
 * 
 * @param device Ukazatel na zařízení CUPREXIT.
 */
void getMeas(CUPREXIT_Device *device);

/**
 * @brief Získá měření ze všech zařízení CUPREXIT a odešle je.
 * 
 * @param CU_devices Pole zařízení CUPREXIT.
 * @param brain_uid Ukazatel na UID mozku.
 */
void getAllMeasAndSend(CUPREXIT_Device CU_devices[], uint8_t *brain_uid);

/**
 * @brief Získá stav zařízení CUPREXIT.
 * 
 * @param device Ukazatel na zařízení CUPREXIT.
 * @return uint8_t Stav zařízení.
 */
uint8_t getStatus(CUPREXIT_Device *device);

/**
 * @brief Provede měření.
 */
void meas();

/**
 * @brief Nastaví uživatelské ID.
 */
void setUserId(CUPREXIT_Device *device, uint8_t user_id);

/**
 * @brief Získá uživatelské ID.
 */
uint8_t getUserId(CUPREXIT_Device *device);

/**
 * @brief Získá UID zařízení CUPREXIT.
 * 
 * @param device Ukazatel na zařízení CUPREXIT.
 */
void getUid(CUPREXIT_Device *device);


void resetCu(CUPREXIT_Device *device);
#endif // CUPREXIT_H
