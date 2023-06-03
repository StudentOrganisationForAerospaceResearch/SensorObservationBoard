

/**
 ******************************************************************************
 * File Name          : LoadCellTask.hpp
 * Description        : Primary LoadCell task, default task for the system.
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
	LOADCELL_REQUEST_TARE,		  // Send the current load cell data during tare over the Debug UART
	LOADCELL_REQUEST_CALIBRATE,   // Send the current load cell data during calibration over the Debug UART
    LOADCELL_REQUEST_NEW_SAMPLE,  // Get a new load cell sample, task will be blocked for polling time
    LOADCELL_REQUEST_TRANSMIT,    // Send the current load cell data over the Radio
	LOADCELL_REQUEST_DUMP_DATA,
	LOADCELL_REQUEST_DUMP_DATA_STOP,
    LOADCELL_REQUEST_DEBUG        // Send the current load cell data over the Debug UART
};

struct LoadCellSample
{
	float weight_g;
	//float knownmass_lb = 16.5347; //This weight it in pounds and refers to the aluminum plate.
};

class LoadCellTask : public Task
{
public:
    static LoadCellTask& Inst() {
        static LoadCellTask inst;
        return inst;
    }

    void InitTask();

protected:
    static void RunTask(void* pvParams) { LoadCellTask::Inst().Run(pvParams); } // Static Task Interface, passes control to the instance Run();

    void Run(void * pvParams); // Main run code


    void HandleCommand(Command& cm);
    void HandleRequestCommand(uint16_t taskCommand);

    void SampleLoadCellData();
    void SampleDumpLoadCellData();
    void LoadCellTare();
    void LoadCellCalibrate(float known_mass_g);
    int32_t GetNoLoad() {return loadcell.offset; }

    hx711_t loadcell;
    bool is_dump_sample = false;
    LoadCellSample loadCellSample;



private:
    // Private Functions
    LoadCellTask();        // Private constructor
    LoadCellTask(const LoadCellTask&);                        // Prevent copy-construction
    LoadCellTask& operator=(const LoadCellTask&);            // Prevent assignment
};

#endif    // SOAR_LOADCELLTASK_HPP_

