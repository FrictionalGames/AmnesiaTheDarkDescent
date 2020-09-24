
//! File: <HXMouse.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_HXMOUSE_H_
#define _HAPTX_HXMOUSE_H_

#include <setupapi.h>
#include <HaptX.h>

namespace HaptX
{

extern "C"
{			
	// The HXMouse class communicates with the HaptX Mouse Driver and therefore uses
	// datastructures from the DDK

	#if _MSC_VER < 1400 //2007-05-16 Frasse. SpecStrings.h is not included in vs 2003.
		#include <DDK/SpecStrings.h>
	#endif
	#include <DDK/hidsdi.h>					
}

// <NO_DOC>

// typedefs used internally for communication with the lowlevel driver.
typedef LIST_ENTRY LIST_NODE_HDR, *PLIST_NODE_HDR;
typedef LIST_NODE_HDR LIST, *PLIST;

#define InitializeList(nl) { ((PLIST)(nl)) -> Flink = ((PLIST)(nl)) -> Blink = nl; }

static LIST					PhysicalDeviceList;

// defines used when converting touch controller movement to mouse movement
#define MOU_X_MAX			0.05f
#define MOU_X_MIN			-0.05f

// struct used internally for communication with the lowlevel driver.
typedef struct _HID_DATA
{
	BOOLEAN     IsButtonData;
	UCHAR       Reserved;
	USAGE       UsagePage;		// The usage page for which we are looking.
	ULONG       Status;			// The last status returned from the accessor function
								// when updating this field.
	ULONG       ReportID;		// ReportID for this given data structure
	BOOLEAN     IsDataSet;		// Variable to track whether a given data structure
								//  has already been added to a report structure

	union
	{
		struct
		{
			ULONG       UsageMin;       // Variables to track the usage minimum and max
			ULONG       UsageMax;       // If equal, then only a single usage
			ULONG       MaxUsageLength; // Usages buffer length.
			PUSAGE      Usages;         // list of usages (buttons ``down'' on the device.
		} ButtonData;
      
		struct
		{
			USAGE       Usage; // The usage describing this value;
			USHORT      Reserved;

			ULONG       Value;
			LONG        ScaledValue;
		} ValueData;
	};
} HID_DATA, *PHID_DATA;
 
// struct used internally for communication with the lowlevel driver.
typedef struct _HID_DEVICE
{   
	PWCHAR               DevicePath;
    HANDLE               HidDevice;			// A file handle to the hid device.
    BOOL                 OpenedForRead;
    BOOL                 OpenedForWrite;
    BOOL                 OpenedOverlapped;
    BOOL                 OpenedExclusive;
    
    PHIDP_PREPARSED_DATA Ppd;				// The opaque parser info describing this device
    HIDP_CAPS            Caps;				// The Capabilities of this hid device.
    HIDD_ATTRIBUTES      Attributes;

    PCHAR                InputReportBuffer;
    PHID_DATA            InputData;			// array of hid data structures
    ULONG                InputDataLength;	// Num elements in this array.
    PHIDP_BUTTON_CAPS    InputButtonCaps;
    PHIDP_VALUE_CAPS     InputValueCaps;

    PCHAR                OutputReportBuffer;
    PHID_DATA            OutputData;
    ULONG                OutputDataLength;
    PHIDP_BUTTON_CAPS    OutputButtonCaps;
    PHIDP_VALUE_CAPS     OutputValueCaps;

    PCHAR                FeatureReportBuffer;
    PHID_DATA            FeatureData;
    ULONG                FeatureDataLength;
    PHIDP_BUTTON_CAPS    FeatureButtonCaps;
    PHIDP_VALUE_CAPS     FeatureValueCaps;
} HID_DEVICE, *PHID_DEVICE;

// struct used internally for communication with the lowlevel driver.
typedef struct _DEVICE_LIST_NODE
{
    LIST_NODE_HDR   Hdr;
    HID_DEVICE      HidDeviceInfo;
    BOOL            DeviceOpened;

} DEVICE_LIST_NODE, *PDEVICE_LIST_NODE;

// exception class used in the contrstructor of HXMouse if the mouse driver has not been installed.
class CDrivException
{
public:
	CDrivException()	{}
	~CDrivException()	{}

	const char* ShowReason() const
	{ 
		return "HaptX Mouse Driver not installed.";
	}
};
// </NO_DOC>

// HXMouse takes input from a touch controller. The movement and button clicks
// is then sent to the HaptX Mouse Driver. Windows periodically polls the mouse 
// driver for changes.
// This allowes the end user to use the touch controller as a mouse in Windows.
class HXMouse
{
public:
	HXMouse(float SpeedCoef = 1.0f);
	~HXMouse();
	HX_RESULT	UpdatePosition(float dx, float dy, int Buttons);	//Funciton used to report movements and the state of the buttons on the touch controller.

private:
	void		SetXPos(double Curr, double Min, double Max);		// Convert touch controller movement to mouse movement along the X-axis.
	void		SetYPos(double Curr, double Min, double Max);		// Convert touch controller movement to mouse movement along the Y-axis.
	void		SetButtons(int Buttons);							// Convert touch controller button states to mouse button states.
	void		SendPosToDriver();									// Sends recent data down to the lowlevel driver.
	void		ClearXAndY();										// Clear all mouse movement.
	double		GetMouPos(double XCurr, double XMin, double XMax);	// Convert touch controller movement to mouse movement.
	// Functions used internally for communication with the lowlevel driver.
	int			FindDriver();										
	void		CloseDriver(HANDLE Driver);							
	int			FindKnownHidDevices(OUT PHID_DEVICE* HidDevices, OUT PULONG NumberDevices);	
	int			OpenHidDevice (IN PWCHAR DevicePath, IN BOOL HasReadAccess, IN BOOL HasWriteAccess, IN BOOL IsOverlapped, IN BOOL IsExclusive, IN OUT PHID_DEVICE HidDevice);
	int			FillDeviceInfo(IN PHID_DEVICE HidDevice);			
	void		CloseHidDevices(OUT PHID_DEVICE HidDevices, OUT ULONG NumberDevices);	
	void		CloseHidDevice(IN PHID_DEVICE HidDevice);

private:
	char				dX;											// Current movement along the X-axis.
	char				dY;											// Current movement along the Y-axis.
	UCHAR				m_Buttons;									// Current state of the mouse buttons.
	float				m_SpeedCoef;								// Coefficient used to increase or decrease mouse sensitivity.
	PDEVICE_LIST_NODE	m_MouseDevice;								// Variable used internally for communication with the lowlevel driver.
};

extern HXMouse* MouseDriverObj;										// Global pointer to a HXMouse object.
}

#endif
