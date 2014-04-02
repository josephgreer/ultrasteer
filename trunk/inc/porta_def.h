////////////////////////////////////////////////////////////////////////////////
///@file porta_def.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

/// Callback used when an image has finished processing.
///
///@param           prm Custom user parameter that is passed from the callback.
///@param           id The frame identifer originally from the cine buffer.
///@return          A boolean value. Reserved for future use.
///@sa              porta::setDisplayCallback()
typedef int (*PORTA_IMG_CALLBACK)(void* prm, int id, int header);

/// Callback used when raw ultrasound data has been acquired in cine memory.
///
///@param           prm Custom user parameter that is passed from the callback.
///@param           data Pointer to the cine data.
///@param           cineBlock The cine block index where the data came from.
///                 The number of imaging blocks depends on the current imaging mode.
///@param           header The data information header.
///@return          A boolean value. Reserved for future use.
///@sa              porta::setRawDataCallback()
///@sa              imagingMode for information on cine block indexing.
typedef int (*PORTA_DATA_CALLBACK)(void* prm, unsigned char* data, int cineBlock, int header);

/// Callback used prior to running the image when Porta makes internal calls to the function.
///
typedef int (*PORTA_PRERUN_CALLBACK)(void* prm);

/// Definition for a rectangle
struct portaRect
{
    /// left boundary
    int left;
    /// top boundary
    int top;
    /// right boundary
    int right;
    /// bottom boundary
    int bottom;
};

/// Definition of a point
struct portaPoint
{
    /// X co-ordinate
    int x;
    /// Y co-ordinate
    int y;
};

/// Definition for Ultrasonix 3-point curve
struct portaCurve
{
    /// horizontal position of top point
    int t;
    /// horizontal position of middle point
    int m;
    /// horizontal position of bottom point
    int b;
    /// vertical position of middle point
    int vm;
};

/// Stores an 8 point gain curve value
struct portaGainCurve
{
    /// Array of 8 integer values that will range from 0 - 255
    int val[8];
};

/// Stores probe information useful to the calling program
struct probeInfo
{
    /// The number of elements the probe has.
    int elements;
    /// The distance in microns between elements.
    int pitch;
    /// The radius of the probe if it is a convex or concave array.
    int radius;
    /// Specifies whether the probe has a motor or not.
    bool motorized;
    /// The field of view of the motor if applicable.
    int motorFov;
    /// The radius of the motor if applicable.
    int motorRadius;
    /// The number of steps in the motor sweep if applicable.
    int motorSteps;
    /// Specifies whether the motorized probe has a homing sensor
    bool motorHomeSensor;
    // specified whether the probe has angle/temperature sensor (for TEE only)
    bool supportsTempAngleSensor;
};

enum portaVarType
{
    pVariableUnknown = -1,
    pInteger = 0,
    pFloat = 1,
    pString = 2,
    pGainCurve = 3,
    pRectangle = 4,
    pCurve = 5,
    pColor = 6,
    pBoolean = 7,
    pArray = 8,
    pPoint = 9,
    pScanConvert = 10,
    pVariableEnd
};
