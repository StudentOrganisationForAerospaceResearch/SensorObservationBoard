/**
 ******************************************************************************
 * File Name          : LoadCellTask.hpp
 * Description        : Primary LoadCell task for reading rocket mass
 ******************************************************************************
*/
#ifndef SOAR_LOADCELLTASK_HPP_
#define SOAR_LOADCELLTASK_HPP_
#define LBS_TO_GRAMS(lbs) ((lbs) * 453.592)
#define GRAMS_TO_LBS(grams) ((grams) * 0.00220462)
#include "Task.hpp"
#include "SystemDefines.hpp"
#include "hx711.h"


/* Macros/Enums ------------------------------------------------------------*/
enum LOADCELL_TASK_COMMANDS {
    LOADCELL_NONE = 0,
	LOADCELL_REQUEST_TARE,		  			// Send the current load cell data during tare over the Debug UART
	LOADCELL_REQUEST_CALIBRATE,   			// Calibrate load cell with known mass (in 10^-2 grams)
    LOADCELL_REQUEST_NEW_SAMPLE,  			// Get a new load cell sample, task will be blocked for polling time
    LOADCELL_REQUEST_TRANSMIT,    		 	// Send the current load cell data over the Radio
	LOADCELL_REQUEST_CALIBRATION_DEBUG, 	// Print the offset, scale, and known mass used for calibration
    LOADCELL_REQUEST_DEBUG        			// Send the current load cell data over the Debug UART
};

struct LoadCellSample
{
	float weight_g;
	uint32_t timestamp_ms;
};

class LoadCellTask : public Task
{
public:
    static LoadCellTask& Inst() {
        static LoadCellTask inst;
        return inst;
    }

    void InitTask();
    void SetCalibrationMassGrams(const float mass_g) { calibration_mass_g = mass_g; };
    const float getCalibrationMassGrams() { return calibration_mass_g; };

protected:
    static void RunTask(void* pvParams) { LoadCellTask::Inst().Run(pvParams); } // Static Task Interface, passes control to the instance Run();

    void Run(void * pvParams); // Main run code


    void HandleCommand(Command& cm);
    void HandleRequestCommand(uint16_t taskCommand);

    void SampleLoadCellData();
    void LoadCellTare();
    void LoadCellCalibrate();
    void TransmitProtocolLoadCellData();

    hx711_t loadcell;
    LoadCellSample rocket_mass_sample;
    float calibration_mass_g;

private:
    // Private Functions
    LoadCellTask();        // Private constructor
    LoadCellTask(const LoadCellTask&);                        // Prevent copy-construction
    LoadCellTask& operator=(const LoadCellTask&);             // Prevent assignment
};

#endif    // SOAR_LOADCELLTASK_HPP_
