////////////////////////////////////////////////////////////////////////////////
///@file porta_params_def.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////////////////////////
/// Imaging Parameters Definition.
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// Acoustic Power:
/// \li \b Functionality: Adjusts the transmission power of the transducer.
/// \li \b Type: Integer
/// \li \b Units: Index
/// \li \b Range: 0 to 15. Regular Power Electronics: +\\-6V to +\\-47V, with an array of 16 choices.
/// \par
/// Positive Values { 6, 6, 7, 7, 7, 7, 7, 9, 16, 18, 22, 25, 25, 32, 42, 47 }
/// \par
/// Negative Values { -6, -6, -7, -7, -7, -7, -7, -9, -16, -19, -22, -26, -26, -32, -42, -47 }
/// \li \b Range: High Voltage Power Electronics: +\\-5V to +\\-95V, with an array of 16 choices.
/// \par
/// Positive Values { 5, 9, 19, 25, 28, 32, 36, 39, 47, 52, 55, 68, 71, 76, 87, 95 }
/// \par
/// Negative Values { -5, -9, 19, -25, -28, -32, -36, -39, -47, -52, -55, -68, -71, -76, -87, -95 }
/// @note   The internal power settings are programmed through other parameters with power index
///         values for positive and negative. The Acoustic Power parameter offsets those indexes.
///         Typically the Acoustic Power value is always set to 0 in the master preset.
/// \par    Example 1:
///         Positive power index = 11 (25V). Negative power index = 10 (-22V) \n
///         Acoustic Power = 0 \n
///         Result = Positive power index = 11 (25V). Negative power index = 10 (-22V)
/// \par    Example 2:
///         Positive power index = 11 (25V). Negative power index = 10 (-22V) \n
///         Acoustic Power = 3 \n
///         Result = Positive power index = 8 (16V). Negative power index = 7 (-9V)
///
////////////////////////////////////////////////////////////////////////////////
#define prmAcousticPower "power"
////////////////////////////////////////////////////////////////////////////////
/// Image Depth:
/// \li \b Functionality: Adjusts the depth of imaging for B and RF acquisitions.
/// \li \b Type: Integer
/// \li \b Units: Millimeters
/// \li \b Range: 20 to 300 mm. Dependant on transducer, see imaging.set.xml.
///
////////////////////////////////////////////////////////////////////////////////
#define prmBImageDepth "b-depth"
////////////////////////////////////////////////////////////////////////////////
/// Image Depth BB:
/// \li \b Functionality: Adjusts the depth of imaging for B and RF acquisitions.
/// \li \b Type: Integer
/// \li \b Units: Millimeters
/// \li \b Range: 20 to 300 mm. Dependant on transducer, see imaging.set.xml.
///
////////////////////////////////////////////////////////////////////////////////
#define prmBBImageDepth "bb-depth"
////////////////////////////////////////////////////////////////////////////////
/// Image Depth 4B A:
/// \li \b Functionality: Adjusts the depth of imaging for B and RF acquisitions.
/// \li \b Type: Integer
/// \li \b Units: Millimeters
/// \li \b Range: 20 to 300 mm. Dependant on transducer, see imaging.set.xml.
///
////////////////////////////////////////////////////////////////////////////////
#define prm4BImageDepthA "4b-depth a"
////////////////////////////////////////////////////////////////////////////////
/// Image Depth 4B A:
/// \li \b Functionality: Adjusts the depth of imaging for B and RF acquisitions.
/// \li \b Type: Integer
/// \li \b Units: Millimeters
/// \li \b Range: 20 to 300 mm. Dependant on transducer, see imaging.set.xml.
///
////////////////////////////////////////////////////////////////////////////////
#define prm4BImageDepthB "4b-depth b"
////////////////////////////////////////////////////////////////////////////////
/// Image Zoom:
/// \li \b Functionality: Adjusts the zoom level of the B image, and any overlay that is associated.
/// \li \b Type: Integer
/// \li \b Units: Percentage
/// \li \b Range: 10 to 200 %, where 100% scales the image so that the full height exactly fits
///        the specified display area.
///@note    The image is zoomed around its center. A pan or HD zoom can be implemented by
///         making use of some other internal parameters.
///@sa      porta::setDisplayDimensions()
///
////////////////////////////////////////////////////////////////////////////////
#define prmZoom "zoom user"
////////////////////////////////////////////////////////////////////////////////
/// TGC Curve:
/// \li \b Functionality: Adjusts the analog time-gain compensation curve for variable gain
///        as a function of image depth.
/// \li \b Type: Integer Array [8 values]
/// \li \b Units: Custom
/// \li \b Range: 0 to 255.
///@note    See "Gain Description.doc" for a more in depth description of how
///         the gain system works.
///
////////////////////////////////////////////////////////////////////////////////
#define prmTGC "gain curve"
////////////////////////////////////////////////////////////////////////////////
/// Brightness:
/// \li \b Functionality: Adjusts the post-processing map curve by adjusting the brightness slope.
/// \li \b Type: Integer
/// \li \b Units: Custom
/// \li \b Range: -127 to 127
///
////////////////////////////////////////////////////////////////////////////////
#define prmBrightness "brightness"
////////////////////////////////////////////////////////////////////////////////
/// Contrast
/// \li \b Functionality: Adjusts the post-processing map curve by adjusting the contrast slope.
/// \li \b Type: Integer
/// \li \b Units: Custom
/// \li \b Range: -127 to 127
///
////////////////////////////////////////////////////////////////////////////////
#define prmContrast "contrast"
////////////////////////////////////////////////////////////////////////////////
/// Gamma:
/// \li \b Functionality: Adjusts the post-processing map curve by adjusting the gamme curve.
/// \li \b Type: Integer
/// \li \b Units: Custom
/// \li \b Range: 1 to 500
///
////////////////////////////////////////////////////////////////////////////////
#define prmGamma "gamma"
////////////////////////////////////////////////////////////////////////////////
/// B Mode Map:
/// \li \b Functionality: Optimizes brightness, contrast and gamma to pre-determined values.
/// \li \b Type: Integer (Optimization)
/// \li \b Units: Index
/// \li \b Range: optMap1 to optMap9
///@sa      optBMap
////////////////////////////////////////////////////////////////////////////////
#define prmBMap "b map"
////////////////////////////////////////////////////////////////////////////////
/// Sector:
/// \li \b Functionality: Adjusts the width of the B image.
/// \li \b Type: Integer
/// \li \b Units: Percentage
/// \li \b Range: 10 to 200 %. Dependant on transducer, see imaging.set.xml.
///@note    The number of scanlines can be determined by multiplying the set line density
///         by the sector percentage.\n
///         When Sector is greater than 100% the image is put into extended field
///         of view mode, where the image is full width but has some steering on the edges.
///
////////////////////////////////////////////////////////////////////////////////
#define prmBSector "sector"
////////////////////////////////////////////////////////////////////////////////
/// Sector:
/// \li \b Functionality: Adjusts the width of the B image.
/// \li \b Type: Integer
/// \li \b Units: Percentage
/// \li \b Range: 10 to 200 %. Dependant on transducer, see imaging.set.xml.
///@note    The number of scanlines can be determined by multiplying the set line density
///         by the sector percentage.\n
///         When Sector is greater than 100% the image is put into extended field
///         of view mode, where the image is full width but has some steering on the edges.
///
////////////////////////////////////////////////////////////////////////////////
#define prmBBSector "bb-sector"
////////////////////////////////////////////////////////////////////////////////
/// Sector:
/// \li \b Functionality: Adjusts the width of the B image.
/// \li \b Type: Integer
/// \li \b Units: Percentage
/// \li \b Range: 10 to 200 %. Dependant on transducer, see imaging.set.xml.
///@note    The number of scanlines can be determined by multiplying the set line density
///         by the sector percentage.\n
///         When Sector is greater than 100% the image is put into extended field
///         of view mode, where the image is full width but has some steering on the edges.
///
////////////////////////////////////////////////////////////////////////////////
#define prm4BSectorA "4b-sector a"
////////////////////////////////////////////////////////////////////////////////
/// Sector:
/// \li \b Functionality: Adjusts the width of the B image.
/// \li \b Type: Integer
/// \li \b Units: Percentage
/// \li \b Range: 10 to 200 %. Dependant on transducer, see imaging.set.xml.
///@note    The number of scanlines can be determined by multiplying the set line density
///         by the sector percentage.\n
///         When Sector is greater than 100% the image is put into extended field
///         of view mode, where the image is full width but has some steering on the edges.
///
////////////////////////////////////////////////////////////////////////////////
#define prm4BSectorB "4b-sector b"
////////////////////////////////////////////////////////////////////////////////
/// B Persistence:
/// \li \b Functionality: Adjusts the amount of frame averaging.
/// \li \b Type: Integer
/// \li \b Units: Index
/// \li \b Range: 0 to 6, where 0 specifies no frame averaging.
///
////////////////////////////////////////////////////////////////////////////////
#define prmBPersist "b-persistence"
////////////////////////////////////////////////////////////////////////////////
/// Dynamic Range:
/// \li \b Functionality: Adjusts the log compression curve used in scaling the
///        data after envelope detection from 16 to 8 bits.
/// \li \b Type: Integer
/// \li \b Units: Custom
/// \li \b Range: 30 to 90.
///@note    The true dB value is not calculated with this parameter, even though
///         the Sonix clinical software shows the value in dB.
///
////////////////////////////////////////////////////////////////////////////////
#define prmBDynRange "b-dyn log factor"
////////////////////////////////////////////////////////////////////////////////
/// B Image Steering:
/// \li \b Functionality: Steers the B image.
/// \li \b Type: Integer
/// \li \b Units: 1/1000th of Degree
/// \li \b Range: Dependant on transducer, see imaging.set.xml.
///
////////////////////////////////////////////////////////////////////////////////
#define prmBSteer "b-deviation"
////////////////////////////////////////////////////////////////////////////////
/// General Gain:
/// \li \b Functionality: Adjusts the overall analog gain for the entire image.
/// \li \b Type: Integer
/// \li \b Units: Custom
/// \li \b Range: -3000 to 3000
///@note    See "Gain Description.doc" for a more in depth description of how
///         the gain system works.
///
///////////////////////////////////////////////////////////////////////////////////
#define prmBGain "b-gain"
////////////////////////////////////////////////////////////////////////////////
/// Transmit Frequency:
/// \li \b Functionality: Adjusts the transmit frequency of the transducer.
/// \li \b Type: Integer
/// \li \b Units: Hz
/// \li \b Range: Dependant on transducer, see imaging.set.xml.
///@note    The transmit wave resolution is 80 MHz, so only a specific range of
///         values will actually produce an output that matches the input.
////////////////////////////////////////////////////////////////////////////////
#define prmBTxFreq "b-freq"
////////////////////////////////////////////////////////////////////////////////
/// Focus Depth:
/// \li \b Functionality: Adjusts the focal depth of all the focal points.
/// \li \b Type: Integer
/// \li \b Units: Microns
/// \li \b Range: 5 mm and beyond, dependant on current imaging depth.
///@note        If there is an odd number of focal points, then the focal depth
///             will specify the central focal point. If there is an even number
///             of focal points, then the focal depth will specify the imaginary
///             point between the two central focal points.
////////////////////////////////////////////////////////////////////////////////
#define prmBFocusDepth "focus depth"
////////////////////////////////////////////////////////////////////////////////
/// Focus Count:
/// \li \b Functionality: Adjusts the number of focal points.
/// \li \b Type: Integer
/// \li \b Units: Count
/// \li \b Range: 1 to 10, dependant on transducer, see imaging.set.xml.
///
////////////////////////////////////////////////////////////////////////////////
#define prmBFocusCount "b-focus count"
////////////////////////////////////////////////////////////////////////////////
/// Focus Spacing:
/// \li \b Functionality: Adjusts the spacing betwen focal points.
/// \li \b Type: Integer
/// \li \b Units: Microns
/// \li \b Range: 1 to 100 mm
///
////////////////////////////////////////////////////////////////////////////////
#define prmBFocusSpacing "focus span"
////////////////////////////////////////////////////////////////////////////////
/// B Image Optimization:
/// \li \b Functionality: Optimizes parameters such as transmit frequency and receive
///        filters as to adjust imaging for certain patient/tissue types.
/// \li \b Type: Integer (Optimization)
/// \li \b Units: Index
/// \li \b Range: optPenetration, optGeneral, optResolution, optHarmonics, optHarmonicsPen
///@sa      optBImage
///@note    Certain transducers have harmonic imaging operating by transmitting at a
///         low frequency and then receive filtering at twice the frequency.\n
///         Other transducers transmit two inverse pulses and then average them in
///         the RF domain as to create a harmonic image.
///
////////////////////////////////////////////////////////////////////////////////
#define prmBImageOpt "b image"
////////////////////////////////////////////////////////////////////////////////
/// MRU Filtering:
/// \li \b Functionality: Optimizes the MRU filtering function.
/// \li \b Type: Integer (Optimization)
/// \li \b Units: Index
/// \li \b Range: optMruOff, optMruMed, optMruHigh, optMruMax
///@note    MRU filtering is a special filter that can only be activated by purchasing
///         a dongle from Ultrasonix. It takes some processing time, so if acquiring
///         raw data for real-time processing is critical, or processed data at high
///         frame rates, then it should be ensured that this filter is turned off if
///         enabled on the system.
////////////////////////////////////////////////////////////////////////////////
#define prmBMru "mru filtering"
////////////////////////////////////////////////////////////////////////////////
/// B Extension Angle:
/// \li \b Functionality: Adjusts the angle on both sides for trapezoidal imaging, for larger field of view.
/// \li \b Type: Integer
/// \li \b Units: Index
/// \li \b Range: 0 to angle amount
///@note    0 = no extension.
///
////////////////////////////////////////////////////////////////////////////////
#define prmBExtensionAngle "b-extension angle"
////////////////////////////////////////////////////////////////////////////////
/// Dual Mode Active Display:
/// \li \b Functionality: Adjusts the active display in dual modes.
/// \li \b Type: Integer
/// \li \b Units: Index
/// \li \b Range: 0 and 1
///
////////////////////////////////////////////////////////////////////////////////
#define prmDualActiveDisplay "bb-live"
////////////////////////////////////////////////////////////////////////////////
/// Quad Mode Active Display:
/// \li \b Functionality: Adjusts the active display in quad B mode.
/// \li \b Type: Integer
/// \li \b Units: Index
/// \li \b Range: 0 to 3
///
////////////////////////////////////////////////////////////////////////////////
#define prmQuadActiveDisplay "4b-live"

////////////////////////////////////////////////////////////////////////////////
/// M Mode:
/// \li \b Functionality: Switches the M mode from B/M to M.
/// \li \b Type: Integer
/// \li \b Units: Index
/// \li \b Range: 0 and 1.
///
////////////////////////////////////////////////////////////////////////////////
#define prmMMode "m-mode"
////////////////////////////////////////////////////////////////////////////////
/// M Zoom:
/// \li \b Functionality: Adjusts the size of the M acquisition.
/// \li \b Type: Integer
/// \li \b Units: Microns
/// \li \b Range: 10 mm and beyond, dependant on current imaging depth.
///
////////////////////////////////////////////////////////////////////////////////
#define prmMZoom "m-mline size"
////////////////////////////////////////////////////////////////////////////////
/// M Line Position:
/// \li \b Functionality: Adjusts the horizontal position of the M line.
/// \li \b Type: Integer
/// \li \b Units: Element
/// \li \b Range: 0 to 127, dependant on the current transducer.
///
////////////////////////////////////////////////////////////////////////////////
#define prmMPos "m-mline pos"
////////////////////////////////////////////////////////////////////////////////
/// M Imaging Depth:
/// \li \b Functionality: Adjusts the depth at which the M acquisition will start.
/// \li \b Type: Integer
/// \li \b Units: Microns
/// \li \b Range: 0 mm and beyond, dependant on current imaging depth.
///
////////////////////////////////////////////////////////////////////////////////
#define prmMDepth "m-mline depth"
////////////////////////////////////////////////////////////////////////////////
/// M Sweep Speed:
/// \li \b Functionality: Adjusts the acquisition rate for the M spectrum.
/// \li \b Type: Integer
/// \li \b Units: Microseconds
/// \li \b Range: 4, 8, 12 ms. Can easily be re-programmed from the imaging.set.xml file.
///
////////////////////////////////////////////////////////////////////////////////
#define prmMSweep "m-lrp"
////////////////////////////////////////////////////////////////////////////////
/// M Steer:
/// \li \b Functionality: Steers the M line.
/// \li \b Type: Integer
/// \li \b Units: 1/1000th of Degree
/// \li \b Range: Dependant on transducer, see imaging.set.xml.
///
////////////////////////////////////////////////////////////////////////////////
#define prmMSteer "m-deviation"

////////////////////////////////////////////////////////////////////////////////
/// PW Active Display:
/// \li \b Functionality: Adjusts the active display(s) in PW mode.
/// \li \b Type: Integer
/// \li \b Units: Index
/// \li \b Range: 0, 1, 2
///@note    0 = B image only. 1 = Spectrum only. 2 = B and Spectrum
///
////////////////////////////////////////////////////////////////////////////////
#define prmPwActiveDisplay "pw-mode"
////////////////////////////////////////////////////////////////////////////////
/// PW Gate Position:
/// \li \b Functionality: Adjusts the horizontal position of the PW gate.
/// \li \b Type: Ingeter
/// \li \b Units: Element
/// \li \b Range: 0 to 127, dependant on the current transducer.
///
////////////////////////////////////////////////////////////////////////////////
#define prmPwGatePos "pw-gate pos"
////////////////////////////////////////////////////////////////////////////////
/// PW Gate Depth:
/// \li \b Functionality: Adjusts the depth at which the PW gate starts.
/// \li \b Type: Integer
/// \li \b Units: Microns
/// \li \b Range: 0 mm and beyond, dependant on current imaging depth.
///
////////////////////////////////////////////////////////////////////////////////
#define prmPwGateDepth "pw-gate depth"
////////////////////////////////////////////////////////////////////////////////
/// PW Gate Size: Adjusts the size of the PW gate.
/// \li \b Functionality:
/// \li \b Type: Integer
/// \li \b Units: Microns
/// \li \b Range: 1 to 100 mm, dependant on transducer, see imaging.set.xml.
///
////////////////////////////////////////////////////////////////////////////////
#define prmPwGateSize "pw-gate size"
////////////////////////////////////////////////////////////////////////////////
/// PW Gain:
/// \li \b Functionality: Adjusts the analog PW gain.
/// \li \b Type: Integer
/// \li \b Units: Percentage
/// \li \b Range: 0 to 100 %
///
////////////////////////////////////////////////////////////////////////////////
#define prmPwGain "pw-gain"
////////////////////////////////////////////////////////////////////////////////
/// PW Invert:
/// \li \b Functionality: Inverts the PW spectrum about the baseline.
/// \li \b Type: Integer
/// \li \b Units: Custom
/// \li \b Range: 0 and 1
///
////////////////////////////////////////////////////////////////////////////////
#define prmPwInvert "pw-invert"
////////////////////////////////////////////////////////////////////////////////
/// PW Steer:
/// \li \b Functionality: Steers the PW gate
/// \li \b Type: Integer
/// \li \b Units: 1/1000th of Degree
/// \li \b Range: Dependant on transducer, see imaging.set.xml.
///
////////////////////////////////////////////////////////////////////////////////
#define prmPWSteer "pw-deviation"
////////////////////////////////////////////////////////////////////////////////
/// PW Sweep Speed:
/// \li \b Functionality: Adjusts the acquisition rate for the PW spectrum.
/// \li \b Type: Integer
/// \li \b Units: Microseconds
/// \li \b Range: 4, 8, 10, 12 ms. Can easily be re-programmed from the imaging.set.xml file.
///
////////////////////////////////////////////////////////////////////////////////
#define prmPwSweep "pw-lrp"
////////////////////////////////////////////////////////////////////////////////
/// PW Transmit Frequency:
/// \li \b Functionality: Adjusts the transmission frequency for PW lines.
/// \li \b Type: Integer
/// \li \b Units: Hz
/// \li \b Range: Dependant on transducer, see imaging.set.xml.
///@note    The transmit wave resolution is 80 MHz, so only a specific range of
///         values will actually produce an output that matches the input.
///
////////////////////////////////////////////////////////////////////////////////
#define prmPwTxFreq "pw-freq"
////////////////////////////////////////////////////////////////////////////////
/// PW Baseline:
/// \li \b Functionality: Adjusts the baseline of the PW spectrum.
/// \li \b Type: Integer
/// \li \b Units: Custom
/// \li \b Range: - 64 to 64, where 0 is the a central baseline.
///@note    The FFT size is 128, the baseline adjustment is based on this.
///
////////////////////////////////////////////////////////////////////////////////
#define prmPwBaseline "pw-baseline"
/**
 * PW Pulse Repetition Period:
 * \li \b Functionality: Adjusts the pulse repetition period (see note).
 * \li \b Type: Integer
 * \li \b Units: Microseconds
 * \li \b Range: 80 to 800 us. Dependant on transducer, see imaging.set.xml.
 * @note     This parameter is in microseconds, however it should typically be
 *           displayed to the user as a frequency. PRF in kHz = (1 / prp) * 1000.
 *
 * \par Velocity Range Calculation:
 * \code
 *
 * // Input: PRP (us), TransmitFrequency (Hz), CorrectionAngle (1/1000th Degree), PI, FFT_Size (typically 128)
 *
 * double PRF_Hz = 1000000.0 / PRP;
 * double SpeedOfSound = 1540.0;
 * double MaxVelocity = (PRF_Hz * SpeedOfSound) / (2.0 * TransmitFrequency);
 * double VelocityPerPixel = 2.0 * (MaxVelocity / FFT_Size);
 * VelocityPerPixel /= (2.0 * cos((CorrectionAngle / 1000.0) * (PI / 180.0)));
 *
 * \endcode
 */
#define prmPwPrp      "bounded-prp pw"

////////////////////////////////////////////////////////////////////////////////
/// Color Box:
/// \li \b Functionality: Adjusts the regoin of interest that color Doppler data
///        will be displayed.
/// \li \b Type: Rectangle.
/// \li \b Units: Horizontal: Elements. Vertical: Samples
/// \li \b Range: Dependant on transducer, imaging depth and sector.
///
////////////////////////////////////////////////////////////////////////////////
#define prmColorBox "color-color rect"
////////////////////////////////////////////////////////////////////////////////
/// Color Steer:
/// \li \b Functionality: Steers the color box.
/// \li \b Type: Integer
/// \li \b Units: 1/1000th of Degree
/// \li \b Range: Dependant on transducer, see imaging.set.xml.
///
////////////////////////////////////////////////////////////////////////////////
#define prmColorSteer "color-deviation"
////////////////////////////////////////////////////////////////////////////////
/// Color Sampling Freq:
/// \li \b Functionality: Sets the sampling frequency of the color doppler data.
/// \li \b Type: Integer
/// \li \b Units: Hz
/// \li \b Range: 1 to 10 MHz, also see imaging.set.xml.
///
////////////////////////////////////////////////////////////////////////////////
#define prmColorSamplingFreq "color-csampl freq"
////////////////////////////////////////////////////////////////////////////////
/// Color Persistence:
/// \li \b Functionality: Adjusts the frame averaging on the color Doppler data.
/// \li \b Type: Integer
/// \li \b Units: Index
/// \li \b Range: 0 to 6, where 0 specifies no frame averaging.
///
////////////////////////////////////////////////////////////////////////////////
#define prmColorPersist "color-persistence"
////////////////////////////////////////////////////////////////////////////////
/// Color Gain:
/// \li \b Functionality: Adjusts the analog color gain.
/// \li \b Type: Integer
/// \li \b Units: Percentage
/// \li \b Range: 0 to 100 %
///
////////////////////////////////////////////////////////////////////////////////
#define prmColorGain "color-gain"
////////////////////////////////////////////////////////////////////////////////
/// Color Invert:
/// \li \b Functionality: Inverts the color map in velocity mode.
/// \li \b Type: Integer
/// \li \b Units: Custom
/// \li \b Range: 0 and 1
///
////////////////////////////////////////////////////////////////////////////////
#define prmColorInvert "color-invert"
////////////////////////////////////////////////////////////////////////////////
/// Color Transmit Frequency:
/// \li \b Functionality: Adjusts the transmission frequency for color lines.
/// \li \b Type: Integer
/// \li \b Units: Hz
/// \li \b Range: Dependant on transducer, see imaging.set.xml.
///@note    The transmit wave resolution is 80 MHz, so only a specific range of
///         values will actually produce an output that matches the input.
///
////////////////////////////////////////////////////////////////////////////////
#define prmColorTxFreq "color-freq"
////////////////////////////////////////////////////////////////////////////////
/// Color Transmit Frequency Pulsed:
/// \li \b Functionality: Adjusts the transmission frequency for color lines.
/// \li \b Type: Integer
/// \li \b Units: Hz
/// \li \b Range: Dependant on transducer, see imaging.set.xml.
///@note    The transmit wave resolution is 80 MHz, so only a specific range of
///         values will actually produce an output that matches the input.
///
////////////////////////////////////////////////////////////////////////////////
#define prmColorTxFreqPulsed "color-freq pulsed"
////////////////////////////////////////////////////////////////////////////////
/// Color Doppler Ensemble:
/// \li \b Functionality: Adjusts the color Doppler ensemble (packet size).
/// \li \b Type: Integer
/// \li \b Units: Lines
/// \li \b Range: 4 to 24.
//
////////////////////////////////////////////////////////////////////////////////
#define prmColorEnsemble "color-ensemble"
////////////////////////////////////////////////////////////////////////////////
/// Color Mode:
/// \li \b Functionality: Optimizes parameters for the color mode.
/// \li \b Type: Integer (Optimization)
/// \li \b Units: Index
/// \li \b Range: optColor, optPower, optTDI
///@sa      optColorMode
///@note    optColor = regular velocity imaging, optPower = power Doppler.
///         optTDI = tissue Doppler imaging.
///
////////////////////////////////////////////////////////////////////////////////
#define prmColorMode "color-mode"
/**
 * Color Pulse Repetition Period:
 * \li \b Functionality: Adjusts the pulse repetition period (see note).
 * \li \b Type: Integer
 * \li \b Units: Microseconds
 * \li \b Range: 100 to 4200 us. Dependant on transducer, see imaging.set.xml.
 * @note     This parameter is in microseconds, however it should typically be
 *           displayed to the user as a frequency. PRF in kHz = (1 / prp) * 1000.
 *
 * \par Velocity Range Calculation:
 * \code
 *
 * // Input: PRP (us), TransmitFrequency (Hz), NumMapEntries (typically 256)
 *
 * double PRF_Hz = 1000000.0 / PRP;
 * double SpeedOfSound = 1540.0;
 * double VelocityMax = (PRF_Hz * SpeedOfSound) / (2 * TransmitFrequency);
 * double VelocityPerPixel = (VelocityMax / NumMapValues);
 *
 * \endcode
 */
#define prmColorPrp  "bounded-prp color"
////////////////////////////////////////////////////////////////////////////////
/// Triplex Active Display:
/// \li \b Functionality: Adjusts the active display(s) in triplex mode.
/// \li \b Type: Integer (Optimization)
/// \li \b Units: Index
/// \li \b Range: optLiveBC, optLiveW, optLiveBCW
///@sa      optTriplex
///@note    optLiveBC = B/Color image only. optLiveW = Spectrum only. optLiveBCW = B/Color and Spectrum
///
////////////////////////////////////////////////////////////////////////////////
#define prmTriplexActiveDisplay "triplex-mode"
////////////////////////////////////////////////////////////////////////////////
/// Sampling Frequency:
/// \li \b Functionality: Adjusts the sampling frequency of the envelope detector.
/// \li \b Type: Integer
/// \li \b Units: Hz
/// \li \b Range: 1 to 20 MHz
///@note    The decimation factor is always from 40 MHz.
///
////////////////////////////////////////////////////////////////////////////////
#define prmBSamplingFreq "b-sampl freq"
////////////////////////////////////////////////////////////////////////////////
/// Line Density:
/// \li \b Functionality: Adjusts the line density when the sector is at 100 %.
/// \li \b Type: Integer
/// \li \b Units: Scanlines
/// \li \b Range: 32 to 512. Dependant on transducer, see imaging.set.xml.
///@note    When the line density is beyond the native probe elements, virtual
///         transducers are created by slightly steering the beam to interpolate
///         between elements.
////////////////////////////////////////////////////////////////////////////////
#define prmBLineDensity "b-ldensity"
////////////////////////////////////////////////////////////////////////////////
/// Image Rectangle:
/// \li \b Functionality: Low-level control for adjusting the sector and depth.
/// \li \b Type: Rectangle
/// \li \b Units: Horizontal: Elements. Vertical: Samples
/// \li \b Range: Dependant on transducer.
///
////////////////////////////////////////////////////////////////////////////////
#define prmBImageRect "b-image rect"
////////////////////////////////////////////////////////////////////////////////
/// Image Rectangle Dual B:
/// \li \b Functionality: Low-level control for adjusting the sector and depth.
/// \li \b Type: Rectangle
/// \li \b Units: Horizontal: Elements. Vertical: Samples
/// \li \b Range: Dependant on transducer.
///
////////////////////////////////////////////////////////////////////////////////
#define prmBBImageRect "bb-image rect"
////////////////////////////////////////////////////////////////////////////////
/// Image Rectangle Quad B:
/// \li \b Functionality: Low-level control for adjusting the sector and depth.
/// \li \b Type: Rectangle
/// \li \b Units: Horizontal: Elements. Vertical: Samples
/// \li \b Range: Dependant on transducer.
///
////////////////////////////////////////////////////////////////////////////////
#define prm4BImageRectA "4b-image rect a"
////////////////////////////////////////////////////////////////////////////////
/// Image Rectangle Quad B:
/// \li \b Functionality: Low-level control for adjusting the sector and depth.
/// \li \b Type: Rectangle
/// \li \b Units: Horizontal: Elements. Vertical: Samples
/// \li \b Range: Dependant on transducer.
///
////////////////////////////////////////////////////////////////////////////////
#define prm4BImageRectB "4b-image rect b"
////////////////////////////////////////////////////////////////////////////////
/// Positive Power Index:
/// \li \b Functionality: Adjusts the power index for positive tranmsit in B mode.
/// \li \b Type: Integer
/// \li \b Units: Index
/// \li \b Range: 0 to 15
///@sa prmAcousticPower
///
////////////////////////////////////////////////////////////////////////////////
#define prmPowerPositive "b-voltage [+]"
////////////////////////////////////////////////////////////////////////////////
/// Negative Power Index:
/// \li \b Functionality: Adjusts the power index for negative tranmsit in B mode.
/// \li \b Type: Integer
/// \li \b Units: Index
/// \li \b Range: 0 to 15
///@sa prmAcousticPower
///
////////////////////////////////////////////////////////////////////////////////
#define prmPowerNegative "b-voltage [-]"
////////////////////////////////////////////////////////////////////////////////
/// Motor Frames:
/// \li \b Functionality: Adjusts the number of frames that will acquired in one sweep
///        of the motor across the 4D transducer.
/// \li \b Type: Integer
/// \li \b Units: Frames
/// \li \b Range: 3 to 125 (always odd as there always is a central frame)
///@note    4D transducers have a limited number of steps, combining this value with
///         the 4D step count could cause the motor to hit the edges of the probe.
///
////////////////////////////////////////////////////////////////////////////////
#define prmMotorFrames "4d-frames/vol"
////////////////////////////////////////////////////////////////////////////////
/// Motor Steps:
/// \li \b Functionality: The number of steps the motor will move while acuqiring a single frame.
/// \li \b Type: Integer
/// \li \b Units: Steps
/// \li \b Range: 2, 4, 8, 16, 32
///@note    4D transducers have a limited number of steps, combining this value with
///         the 4D frame count could cause the motor to hit the edges of the probe.
///
////////////////////////////////////////////////////////////////////////////////
#define prmMotorSteps "4d-steps/frame"
////////////////////////////////////////////////////////////////////////////////
/// Motor Status:
/// \li \b Functionality: Determines if the motor is on or off while scanning in B or RF mode.
/// \li \b Type: Integer
/// \li \b Units: On/Off
/// \li \b Range: 0, 1
///
////////////////////////////////////////////////////////////////////////////////
#define prmMotorStatus "4d-motor status"
////////////////////////////////////////////////////////////////////////////////
/// Motor FOV:
/// \li \b Functionality: The field of view (FOV) of the motor sweep.
/// \li \b Type: Integer
/// \li \b Units: Degrees
/// \li \b Range: 10 to max field of view depending on the probe. (see probes.xml for details)
///
////////////////////////////////////////////////////////////////////////////////
#define prmMotorFOV "4d-fov"
////////////////////////////////////////////////////////////////////////////////
/// B Line Count:
/// \li \b Functionality: Read only parameter for retreiving the number of B lines.
/// \li \b Type: Integer
/// \li \b Units: Scanlines
/// \li \b Range: Depends on sector and line density.
///@note    The B line count can also be determined by multiplying the B line density
///         by the sector percentage.
///
////////////////////////////////////////////////////////////////////////////////
#define prmBNumLines "porta-blines"
////////////////////////////////////////////////////////////////////////////////
/// B Sample Count:
/// \li \b Functionality: Read only parameter for reteiving the number of 8-bit B samples.
/// \li \b Type: Integer
/// \li \b Units: Samples
/// \li \b Range: Depends on imaging depth and B sampling frequencyn.
////////////////////////////////////////////////////////////////////////////////
#define prmBNumSamples "porta-bsamples"
////////////////////////////////////////////////////////////////////////////////
/// RF Line Count:
/// \li \b Functionality: Read only parameter for retreiving the number of RF lines.
/// \li \b Type: Integer
/// \li \b Units: Scanlines
/// \li \b Range: Depends on sector and line density.
///@note    The RF line count can also be determined by multiplying the B line density
///         by the sector percentage.
///
////////////////////////////////////////////////////////////////////////////////
#define prmRfNumLines "porta-rflines"
////////////////////////////////////////////////////////////////////////////////
/// RF Sample Count:
/// \li \b Functionality: Read only parameter for reteiving the number of 16-bit RF samples.
/// \li \b Type: Integer
/// \li \b Units: Samples
/// \li \b Range: Depends on imaging depth and RF decimation.
///@note    The RF sample count can also be determined by the following formula:\n
///         \code NumSamples = ((ImageRect.bottom - ImageRect.top) * 40000) / (SamplingFrequency / 1000) \endcode
////////////////////////////////////////////////////////////////////////////////
#define prmRfNumSamples "porta-rfsamples"
////////////////////////////////////////////////////////////////////////////////
/// Color Doppler Line Count:
/// \li \b Functionality: Read only parameter for retreiving the number of Color lines, including ensemble.
/// \li \b Type: Integer
/// \li \b Units: Scanlines
/// \li \b Range: Depends on color box width, line density, and ensemble.
///
////////////////////////////////////////////////////////////////////////////////
#define prmColorNumLines "porta-colorlines"
////////////////////////////////////////////////////////////////////////////////
/// Color Doppler Sample Count:
/// \li \b Functionality: Read only parameter for reteiving the number of 16-bit color RF samples.
/// \li \b Type: Integer
/// \li \b Units: Samples
/// \li \b Range: Depends on color box height and RF decimation for color mode.
//
////////////////////////////////////////////////////////////////////////////////
#define prmColorNumSamples "porta-colorsamples"
////////////////////////////////////////////////////////////////////////////////
/// Positive Voltage Level:
/// \li \b Functionality: The actual positive voltage of the current setting.
/// \li \b Type: Integer
/// \li \b Units: Volts
/// \li \b Range: 0 - 100
///
////////////////////////////////////////////////////////////////////////////////
#define prmVoltageLevelPos "porta-vpos"
////////////////////////////////////////////////////////////////////////////////
/// Negative Voltage Level:
/// \li \b Functionality: The actual negative voltage of the current setting.
/// \li \b Type: Integer
/// \li \b Units: Volts
/// \li \b Range: -0 - -100
///
////////////////////////////////////////////////////////////////////////////////
#define prmVoltageLevelNeg "porta-vneg"
