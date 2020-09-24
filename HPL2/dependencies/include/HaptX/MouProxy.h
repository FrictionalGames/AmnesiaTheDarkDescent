
//! File: <MouProxy.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_MOUPROXY_H_
#define _HAPTX_MOUPROXY_H_

#include <setupapi.h>
#include <list>

#include <HaptX.h>

using namespace HaptX;

extern "C"
{			
	#include <DDK/hidsdi.h>					// from the DDK
}

DWORD WINAPI MouseWindowThread(LPVOID lpParam);

struct mouseDriverConf_t 
{
	int				angle;
	int				speed;
	int				flags;
	std::wstring	deviceName;
};

struct coordPair_t 
{
	POINT desktopCoord;
	Vec3f hapticCoord; 
};

struct mouseObjectCacheObj_t 
{
	std::list<coordPair_t>	magnetList;
	//int					wndSize;	// The number of pixels the form that's currently beeing dragged has or 0 if no window dragged.
	Vec3f					wndWeight;	// The "weight" of the form that's currently beeing dragged or 0 if no window dragged.
};

extern BOOL						bMouseThreadRunning;
extern HANDLE					MousewindowThread;
extern mouseDriverConf_t		MouseDriverStartInfo;
extern mouseObjectCacheObj_t	MouseObjectCache;

int GetMouseDeviceID();

typedef LIST_ENTRY LIST_NODE_HDR, *PLIST_NODE_HDR;
typedef LIST_NODE_HDR LIST, *PLIST;

#define InitializeList(nl) { ((PLIST)(nl)) -> Flink = ((PLIST)(nl)) -> Blink = nl; }

static LIST					PhysicalDeviceList;

#define MOU_X_BIT			1
#define MOU_Y_BIT			2
#define MOU_X_MAX			0.05f
#define MOU_X_MIN			-0.05f

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

typedef struct _DEVICE_LIST_NODE
{
    LIST_NODE_HDR   Hdr;
    HID_DEVICE      HidDeviceInfo;
    BOOL            DeviceOpened;

} DEVICE_LIST_NODE, *PDEVICE_LIST_NODE;

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

class CMouProxy
{
public:
	CMouProxy();
	~CMouProxy();

	void		SetXPos(double Curr, double Min, double Max);	
	void		SetYPos(double Curr, double Min, double Max);
	void		SetButtons(int Button1, int Button2);
	void		SendPosToDriver();
	char		GetXPos();
	char		GetYPos();
	void		ClearXAndY();
	Vec3f		CollectPos(HaptX::Vectors::Vec3f CurrPos);
	void		GetScreenCoords(HaptX::Vectors::Vec3f aPos, float coeff, int Angle, LPPOINT ResPt);
	double		GetHapticDist(int Dist);
	void		GetHapticCoords(POINT DesktopCoord, float coeff, int Angle, HaptX::Vectors::Vec3f* ResPt);
	bool		TrustSamples();
	void		InvalidateSamples();
	void		CalcDrift(POINT CurrMouse, PPOINT Drift);
	void		InitObjectCache(mouseObjectCacheObj_t* Obj, float coeff, int Angle);
	void		UpdateObjectCache(mouseObjectCacheObj_t* Obj, POINT Drift, float coeff, int Angle);
	void		ClearObjectCache(mouseObjectCacheObj_t* Obj);
	bool		CheckDraging(mouseObjectCacheObj_t* Obj, POINT pt, int Angle);
	double		GetYCoeff(int Angle);
	double		GetZCoeff(int Angle);

private:
	int			FindDriver();
	void		CloseDriver(HANDLE Driver);
	double		GetJoyPos(double XCurr, double XMin, double XMax);
	int			FindKnownHidDevices(OUT PHID_DEVICE* HidDevices, OUT PULONG NumberDevices);
	int			OpenHidDevice (IN PWCHAR DevicePath, IN BOOL HasReadAccess, IN BOOL HasWriteAccess, IN BOOL IsOverlapped, IN BOOL IsExclusive, IN OUT PHID_DEVICE HidDevice);
	int			FillDeviceInfo(IN PHID_DEVICE HidDevice);
	void		CloseHidDevices(OUT PHID_DEVICE HidDevices, OUT ULONG NumberDevices);
	void		CloseHidDevice(IN PHID_DEVICE HidDevice);
	void		FillCache(HWND H, mouseObjectCacheObj_t* Obj, float coeff, int Angle);

private:
	HANDLE					m_hDriver;
	char					m_sDevicePath[MAX_PATH];
	char					X;
	char					Y;
	char					m_LastX;
	char					m_LastY;
	HWND					m_LastH;
	POINT					m_Anchor;
	POINT					m_LastTopPt;
	HaptX::Vectors::Vec3f	m_AnchorPos;
	int						m_iTrustSamples;
	UCHAR					Buttons;
	PDEVICE_LIST_NODE		MouseDevice;
};

#endif
