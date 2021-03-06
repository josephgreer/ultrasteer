#pragma once
#pragma warning(push,0)

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include "IndividualDefines.h"
#include "RTUtil.h"
#include "RTMatrixMath.h"
#include <armadillo>
#include "RTMath.h"

#define PATH_CAT(x) (std::string(ultrasteerPath)+std::string(x)).c_str()
#define BASE_PATH_CAT(x) (std::string(ultrasteerBasePath)+std::string(x)).c_str()
#define STL_PATH_CAT(x) (std::string(stlPath)+std::string(x)).c_str()

#define TRANSDUCER_CALIBRATION_COEFFICIENTS 14.8449, 0.9477, -0.0018, 0.0, 15.0061, 0.0016, 1.00, 0.0, 0.1638, 0.0166, 0.0052, 0.0, 0.0, 0.0, 0.0, 1.0
#define TRANSDUCER_OFFSET 0  
//#define TRANSDUCER_CALIBRATION_COEFFICIENTS -29.7558,	0.9433,	-0.0034, 0.0, -0.087,	0.0033,	1.00, 0.0, -0.7053,	0.0132,	-0.008, 0.0, 0.0, 0.0, 0.0, 1.0
//#define TRANSDUCER_OFFSET 3.5  // measured offset to correct transducer misalignment with curvalinear probe


