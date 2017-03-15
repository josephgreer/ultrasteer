#include <stdio.h>
#include "Common.h"
#include "JacobianControl.h"
#include <stddef.h>
#ifdef _WIN32
#include <Windows.h>
#endif
#ifdef __AVR_ATmega2560__
#include "Arduino.h"
#endif

//#define RUN_JACOBIAN_TEST
//#define RUN_CVX_TEST
//#define RUN_EKF_JACOBIAN_TEST
//#define RUN_BVLS_TEST
//#define RUN_KF_JACOBIAN_TEST

struct DeltaTestData
{
  //Vec2f64 delta_x_desired;
  f64 q_res[6];
};

Matrixf64 <2, N_TURN_ACT> J;

#ifdef RUN_JACOBIAN_TEST
void runTest()
{
  DeltaTestData testData[] = { 
    { 64.441895, 151.287053, 0.379469, 0.436728, 0.000000, 0.00000000 },
    { 202.793802, 184.238494, 1.000000, 0.892389, 0.000000, 52.78551522 },
    { -205.614920, 85.000440, 0.539768, 1.000000, 0.000000, 47.72306829 },
    { -108.004591, 119.326096, 0.269756, 1.000000, 0.000000, 119.32609551 },
    { 135.574656, 116.590587, 0.608693, 1.000000, 0.000000, 116.59058706 },
    { -55.678492, -33.138699, 0.421665, 0.904337, 0.000000, 0.00000000 },
    { -129.316904, -16.930219, 0.073936, 0.855463, 0.000000, 0.00000000 },
    { -46.000939, -139.151386, 0.000000, 0.695682, 0.241914, 0.00000000 },
    { 99.436972, -164.872233, 0.000000, 0.209117, 0.231294, 0.00000000 },
    { -152.396359, -15.858393, 0.000000, 0.567218, 0.635174, 0.00000000 },
    { 205.673242, -3.178112, 0.000000, 0.048448, 0.125578, 0.00000000 },
    { -12.842182, -4.996420, 0.000000, 0.073341, 0.164896, 0.00000000 },
    { -31.628004, -165.404677, 0.086330, 0.000000, 0.569036, 0.00000000 },
    { -103.949324, 43.490096, 0.000000, 0.236316, 0.679807, 0.00000000 },
    { 216.962137, -157.061541, 0.532788, 0.000000, 0.896889, 0.00000000 },
    { -111.533577, 31.592886, 0.208354, 0.000000, 0.805688, 0.00000000 },
    { -220.759753, -36.130330, 0.000000, 0.190704, 1.000000, 69.76129549 },
    { -9.816971, -331.193040, 0.000000, 0.000000, 1.000000, 269.43357841 },
    { 13.782395, -27.606996, 0.034456, 0.000000, 1.000000, 27.60699586 },
    { -75.903143, -97.678829, 0.000000, 0.000000, 1.000000, 115.75897764 },
    { -378.795605, -58.175923, 0.000000, 0.347540, 1.000000, 357.13457813 },
    { 87.753673, -79.146025, 0.000000, 0.013918, 0.894853, 0.00000000 },
    { -35.373749, -2.756243, 0.000000, 0.098374, 0.987266, 0.00000000 },
    { -33.321031, -41.174523, 0.000000, 0.057247, 1.000000, 45.03291074 },
    { 3.174007, -30.532362, 0.000000, 0.000000, 1.000000, 13.52780297 },
    { -132.568491, 237.439053, 0.000000, 0.674135, 0.988707, 0.00000000 },
    { -261.619505, -15.161434, 0.000000, 0.973980, 1.000000, 230.23800923 },
    { -230.418042, -172.144820, 0.000000, 0.889298, 1.000000, 285.62028782 },
    { -136.639111, 80.554301, 0.000000, 1.000000, 1.000000, 122.02999563 },
    { 129.618379, -253.842227, 0.000000, 0.288392, 1.000000, 14.66830462 },
    { 275.070239, 194.453519, 0.118614, 0.000000, 0.150269, 0.00000000 },
    { -139.372818, 54.351545, 0.000000, 0.308268, 0.301637, 0.00000000 },
    { 187.663714, -113.101951, 0.324140, 0.000000, 0.319866, 0.00000000 },
    { -0.133764, 10.988762, 0.307944, 0.000000, 0.288145, 0.00000000 },
    { 64.145642, 227.483226, 0.508508, 0.368543, 0.000000, 0.00000000 },
    { 172.847716, 286.697519, 1.000000, 1.000000, 0.000000, 123.01507159 },
    { 10.662809, -66.770553, 0.930282, 0.807250, 0.000000, 0.00000000 },
    { 14.296744, -133.382321, 0.773503, 0.422208, 0.000000, 0.00000000 },
    { -84.735267, 14.844662, 0.583091, 0.465061, 0.000000, 0.00000000 },
    { 258.526759, -159.333977, 0.999429, 0.005104, 0.000000, 0.00000000 },
    { -3.806925, -7.656277, 0.995859, 0.000000, 0.016998, 0.00000000 },
    { -20.065783, -32.005908, 0.991891, 0.000000, 0.109391, 0.00000000 },
    { 22.015011, -39.361335, 1.000000, 0.000000, 0.171146, 35.93733516 },
    { -63.782427, -28.476047, 0.881646, 0.000000, 0.253350, 0.00000000 },
    { 3.400344, -13.036971, 0.908964, 0.000000, 0.290984, 0.00000000 },
    { 78.553297, -264.350487, 1.000000, 0.000000, 0.810646, 168.66848587 },
    { -35.375026, 1.814034, 0.908944, 0.000000, 0.805410, 0.00000000 },
    { 31.180290, -12.903764, 1.000000, 0.000000, 0.839900, 1.91211683 },
    { -316.304878, 61.588168, 0.120343, 0.000000, 0.662110, 0.00000000 },
    { 83.936281, 50.101061, 0.257869, 0.000000, 0.517481, 0.00000000 },
    { 149.638610, -119.412131, 0.804322, 0.000000, 0.862194, 0.00000000 },
    { 37.023677, 94.203337, 0.760910, 0.000000, 0.590252, 0.00000000 },
    { 29.696394, -382.264341, 1.000000, 0.000000, 1.000000, 240.85625653 },
    { 9.896674, -127.619691, 1.000000, 0.000000, 1.000000, 128.00285058 },
    { 50.577618, -189.163531, 1.000000, 0.000000, 1.000000, 195.80841933 },
    { -69.136591, 230.645397, 0.494251, 0.000000, 0.334184, 0.00000000 },
    { 27.075572, 152.036984, 0.447202, 0.104709, 0.000000, 0.00000000 },
    { 143.146771, 5.937457, 0.813639, 0.121849, 0.000000, 0.00000000 },
    { 53.838031, 154.750979, 1.000000, 0.482778, 0.000000, 59.44317793 },
    { 26.474242, 21.778856, 1.000000, 0.496837, 0.000000, 33.81679412 },
    { -1.105915, -73.195584, 0.891586, 0.285540, 0.000000, 0.00000000 },
    { 363.859429, 34.596431, 1.000000, 0.000000, 0.000000, 295.29175709 },
    { 208.358403, -104.617570, 1.000000, 0.000000, 0.000000, 233.14815084 },
    { -153.085250, 368.796703, 1.000000, 0.989825, 0.000000, 51.82263639 },
    { 233.788205, 321.841606, 1.000000, 1.000000, 0.000000, 396.15426549 },
    { 99.315710, 99.270779, 1.000000, 1.000000, 0.000000, 140.42185656 },
    { -126.984595, -25.099862, 0.646310, 0.927543, 0.000000, 0.00000000 },
    { 133.332059, -360.245271, 0.572066, 0.000000, 0.112396, 0.00000000 },
    { -193.618193, -49.031752, 0.158792, 0.000000, 0.253938, 0.00000000 },
    { 100.665730, -302.531137, 0.783488, 0.000000, 1.000000, 44.08769866 },
    { 71.782952, 67.295278, 0.865813, 0.000000, 0.805735, 0.00000000 },
    { -105.500376, -107.891203, 0.699194, 0.000000, 1.000000, 40.59592483 },
    { 137.570081, 57.955395, 0.959468, 0.000000, 0.832697, 0.00000000 },
    { 15.883829, 7.982381, 0.987656, 0.000000, 0.809654, 0.00000000 },
    { -80.577273, 290.862183, 0.396382, 0.029993, 0.000000, 0.00000000 },
    { 225.411401, -68.188505, 1.000000, 0.000000, 0.152681, 9.81701619 },
    { 79.002454, -5.997779, 1.000000, 0.000000, 0.066913, 71.41702142 },
    { 147.921962, 40.935552, 1.000000, 0.000000, 0.000000, 135.70592937 },
    { 58.970175, -299.496637, 1.000000, 0.000000, 0.574717, 200.81798791 },
    { 141.343893, -161.870869, 1.000000, 0.000000, 0.748497, 203.34283644 },
    { -321.808705, -54.450727, 0.274071, 0.000000, 0.905683, 0.00000000 },
    { 198.382861, 208.932185, 0.468461, 0.000000, 0.302548, 0.00000000 },
    { 5.603261, 2.501682, 0.478858, 0.000000, 0.295326, 0.00000000 },
    { -35.332528, -10.555550, 0.405762, 0.000000, 0.325797, 0.00000000 },
    { 63.030052, 117.617464, 0.407306, 0.013735, 0.000000, 0.00000000 },
    { 167.183678, 150.632751, 1.000000, 0.427232, 0.000000, 14.78652948 },
    { -96.894512, 60.698676, 0.845375, 0.602454, 0.000000, 0.00000000 },
    { -137.070199, 102.046245, 0.649990, 0.897036, 0.000000, 0.00000000 },
    { 131.732796, -114.929068, 0.813436, 0.565264, 0.000000, 0.00000000 },
    { -16.408072, -14.847543, 0.750986, 0.522403, 0.000000, 0.00000000 },
    { 272.884012, -131.472425, 1.000000, 0.021159, 0.000000, 84.32718664 },
    { 99.509822, -15.196823, 1.000000, 0.000000, 0.000000, 95.60223779 },
    { 54.870101, -11.129770, 1.000000, 0.000000, 0.000000, 55.98749686 },
    { -6.575626, -26.877974, 1.000000, 0.000000, 0.066412, 7.74432760 },
    { -18.809967, -14.597915, 0.974045, 0.000000, 0.108553, 0.00000000 },
    { -67.337075, 78.454366, 0.810389, 0.117926, 0.000000, 0.00000000 },
    { 289.092337, 47.888549, 1.000000, 0.000000, 0.000000, 209.39592467 },
    { -329.340991, 103.682360, 0.326300, 0.299305, 0.000000, 0.00000000 },
    { -333.601693, -63.472751, 0.000000, 0.715394, 0.599319, 0.00000000 },
    { -54.020790, 115.864759, 0.000000, 1.000000, 0.558293, 6.12544320 },
  };

#ifdef _WIN32
  char debugStr[200] = { 0 };
#endif

#ifdef __AVR_ATmega2560__
  u32 beginTime = millis();
#endif

  Vecf64<2> test_vec;
  if (&test_vec.x != &test_vec.m_data[0] || &test_vec.y != &test_vec.m_data[1]) {
#ifdef __AVR_ATmega2560__
    Serial.println("Error in alignment .x = " + String((u32)&test_vec.x) + " .m_data[0] = " + String((u32)&test_vec.m_data[0]) + " .y = " + String((u32)&test_vec.y) + " .m_data[1] = " + String((u32)&test_vec.m_data[1]));
#else
    s32 y = 0;
#endif
  }

  JacobianControl *jc = (JacobianControl *)new JacobianBoxConstraintControl();

  f64 initQ[N_TURN_ACT] = { 1,1,1 };
  jc->SetQs(&initQ[0]);


  f64 Jdata[2][N_TURN_ACT] = {
    {400, -200, -200},
    {0, 346.4102, -346.4102}
  };

  Vecf64<N_TURN_ACT> qs;
  Vecf64<N_TURN_ACT> dqs;

  J = Matrixf64<2, N_TURN_ACT>(Jdata);
  f64 maxMaxError = -1e10;
  for (s32 i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
    Vecf64<2> dx(testData[i].q_res[0], testData[i].q_res[1]);

    jc->Update(qs, dqs, dx, J);

    f64 maxError = -1;
    for (s32 j = 0; j < N_TURN_ACT; j++)
      maxError = MAX(maxError, ABS(qs(j) - testData[i].q_res[2 + j]));

    if (maxError > 1e-4)
      s32 y = 0;

#ifdef _WIN32
    sprintf(debugStr, "i=%d maxError=%f\n", i, maxError);
    OutputDebugString(debugStr);
#endif

    if (maxError > maxMaxError)
      maxMaxError = maxError;

#ifdef __AVR_ATmega2560__
    Serial.println("Error " + String(i) + " " + String(maxError,10));// +" qs " + String(qs[0]) + " " + String(qs[1]) + " " + String(qs[2]));
#endif

    jc->SetQs(&testData[i].q_res[2]);

  }
#ifdef __AVR_ATmega2560__
  u32 endTime = millis();
  f64 nTrials = (f64)(sizeof(testData) / sizeof(testData[0]));
  Serial.println("Average time " + String(((f64)endTime - beginTime) / nTrials));
#endif
#ifdef _WIN32
  sprintf(debugStr, "Maximum Error = %f\n", maxMaxError);
  OutputDebugString(debugStr);
#endif
}
#endif


extern "C"
{
#include "cvxgen/solver.h"
  Vars vars;
  Params params;
  Workspace work;
  Settings settings;
}

#ifdef RUN_CVX_TEST
void runTest()
{
#if 0
  DeltaTestData testData[] = {
    { -356.352529, -24.280793, 0.018780, 0.874615, 0.944707, 0.00000059 },
    { 39.941583, -30.465344, 0.034911, 0.746919, 0.904958, 0.00000060 },
    { 5.464396, 19.848427, 0.042387, 0.769383, 0.870124, 0.00000069 },
    { -11.133521, 4.342671, 0.033833, 0.794930, 0.883135, 0.00000061 },
    { -250.165437, 69.565317, 0.000000, 1.000000, 1.000000, 176.60780253 },
    { -48.159266, 266.180382, 0.000000, 1.000000, 0.483904, 174.79734228 },
    { -8.071445, -5.183887, 0.000000, 1.000000, 0.505218, 4.39812936 },
    { -96.691088, -37.479680, 0.000000, 1.000000, 0.707229, 64.99709876 },
    { -97.653469, 57.668323, 0.000000, 1.000000, 0.704440, 113.40454652 },
    { -61.614649, -165.056207, 0.000000, 0.867438, 1.000000, 33.50296444 },
    { 120.332318, 181.612064, 0.082466, 0.911208, 0.519501, 0.00000102 },
    { 23.200744, -120.329147, 0.137323, 0.734382, 0.690036, 0.00000113 },
    { 27.201998, 152.785872, 0.145835, 0.895417, 0.410015, 0.00000093 },
    { 359.624095, -151.925116, 0.755530, 0.386767, 0.339936, 0.00000068 },
    { -169.218421, -319.033908, 0.472408, 0.066205, 0.940345, 0.00000125 },
    { 85.574700, -124.650271, 0.683071, 0.000000, 1.000000, 81.05116577 },
    { 218.705058, 143.106348, 1.000000, 0.000000, 0.575249, 8.06338678 },
    { -135.179675, -51.411166, 0.798467, 0.062210, 0.785871, 0.00000084 },
    { 25.382487, 160.614695, 0.790100, 0.222215, 0.482221, 0.00000054 },
    { -105.649956, 20.883827, 0.616848, 0.343230, 0.542950, 0.00000101 },
    { -47.523285, 176.798845, 0.537787, 0.638165, 0.327510, 0.00000078 },
    { 150.081967, -180.183789, 0.782922, 0.248023, 0.457513, 0.00000114 },
    { -23.223499, 108.738715, 0.743779, 0.423889, 0.319478, 0.00000069 },
    { -78.847048, 23.110356, 0.617251, 0.527836, 0.356711, 0.00000091 },
    { 61.841870, 196.093626, 0.765294, 0.804310, 0.067112, 0.00000087 },
    { 165.101401, 100.788502, 1.000000, 0.899944, 0.000000, 88.82329821 },
    { -183.403986, 215.625218, 0.591518, 1.000000, 0.000000, 180.96483257 },
    { -5.651111, -276.379401, 0.552236, 0.575927, 0.373765, 0.00000926 },
    { -55.727336, -138.660710, 0.458204, 0.421074, 0.619191, 0.00000857 },
    { -66.880874, -118.349005, 0.332565, 0.291815, 0.831576, 0.00000112 },
    { -116.927462, -57.550130, 0.091111, 0.259613, 0.965508, 0.00000059 },
    { -23.164143, 8.315765, 0.054603, 0.293018, 0.974907, 0.00000045 },
    { 17.879601, 388.229545, 0.134300, 0.888377, 0.449544, 0.00000094 },
    { 33.658547, 274.535400, 0.049485, 1.000000, 0.000000, 80.14146566 },
    { -169.505615, -234.780551, 0.000000, 1.000000, 0.695454, 12.26373857 },
    { -235.747892, 37.204263, 0.000000, 1.000000, 0.909584, 222.76579471 },
    { 26.493019, 11.855891, 0.015848, 0.966728, 0.842087, 0.00000048 },
    { 19.142351, 57.649392, 0.005994, 0.992228, 0.701167, 0.00000081 },
    { 41.820435, 30.426235, 0.031840, 0.957439, 0.578545, 0.00000045 },
    { -70.955780, -24.681625, 0.000000, 1.000000, 0.726038, 23.33550392 },
    { 260.390234, -80.821191, 0.359486, 0.591855, 0.551204, 0.00000795 },
    { -273.528967, -72.101378, 0.009917, 0.822039, 0.989526, 0.00000295 },
    { -32.302640, 369.280135, 0.000000, 1.000000, 0.314407, 147.53215734 },
    { 154.426868, -246.086864, 0.321162, 0.579899, 0.604697, 0.00000832 },
    { -334.248451, 41.713511, 0.000000, 1.000000, 0.981666, 53.54337568 },
    { -8.707361, -263.986799, 0.000000, 0.448503, 1.000000, 133.18306036 },
    { 217.333779, -130.426823, 0.330006, 0.046919, 0.974926, 0.00000043 },
    { -147.218828, 165.010776, 0.123688, 0.446821, 0.898483, 0.00000091 },
    { 25.396850, 72.509880, 0.181715, 0.546015, 0.788359, 0.00000111 },
    { -47.811286, -310.313038, 0.000000, 0.000000, 1.000000, 63.67056218 },
    { 335.171199, -59.368711, 0.837928, 0.000000, 1.000000, 59.36871127 },
    { -34.577396, -9.715333, 0.751485, 0.000000, 1.000000, 9.71533326 },
    { -370.267830, 25.928223, 0.000000, 0.143228, 1.000000, 47.37517264 },
    { 45.437311, -188.642721, 0.041977, 0.000000, 1.000000, 139.02722858 },
    { -83.774713, 24.179426, 0.000000, 0.136080, 1.000000, 45.92016844 },
    { 13.003580, 26.423059, 0.052358, 0.194067, 0.981710, 0.00000044 },
    { 199.737823, -160.240376, 0.463815, 0.000000, 1.000000, 86.67797492 },
    { 102.894435, 129.818766, 0.667633, 0.133960, 0.759205, 0.00000111 },
    { 79.794003, 25.433620, 0.804895, 0.108446, 0.660271, 0.00000102 },
    { -31.492937, -258.538068, 0.841804, 0.000000, 1.000000, 103.28568031 },
    { -108.954832, 14.475386, 0.567141, 0.018617, 0.976831, 0.00000033 },
    { -220.529589, -312.624386, 0.018044, 0.000000, 1.000000, 298.14901034 },
    { -25.782075, 11.056923, 0.000000, 0.047145, 1.000000, 10.54891878 },
    { 30.991405, 11.762217, 0.074489, 0.061132, 0.980033, 0.00000063 },
    { 209.409414, -54.178063, 0.577430, 0.000000, 1.000000, 26.08450838 },
    { 35.523224, -57.999358, 0.666241, 0.000000, 1.000000, 57.99935827 },
    { -17.978752, 276.206489, 0.581777, 0.359153, 0.561813, 0.00000839 },
    { 107.977300, -83.137751, 0.772962, 0.160395, 0.603054, 0.00000867 },
    { 317.501132, 136.621667, 1.000000, 0.000000, 0.000000, 75.86147414 },
    { -75.292681, 286.711551, 1.000000, 0.714866, 0.000000, 78.15040082 },
    { 151.840402, 232.852659, 1.000000, 0.999999, 0.000000, 248.19912033 },
    { 158.035323, -244.182023, 1.000000, 0.273785, 0.000000, 14.77154794 },
    { -184.024957, 225.005099, 0.883740, 0.942354, 0.019035, 0.00000060 },
    { 80.337720, 96.452271, 1.000000, 1.000000, 0.000000, 81.31038177 },
    { -307.038493, 169.845569, 0.232411, 1.000000, 0.000000, 169.84556627 },
    { 200.179969, 289.598211, 0.732915, 1.000000, 0.000000, 289.59832476 },
    { 117.691535, -370.401548, 0.836396, 0.274623, 0.343880, 0.00000112 },
    { -55.248096, 60.561517, 0.754356, 0.418117, 0.312548, 0.00000070 },
    { 19.608350, 330.564069, 0.938044, 1.000000, 0.000000, 20.72392453 },
    { 272.879285, -60.585161, 1.000000, 0.558708, 0.000000, 184.56576888 },
    { -114.126269, 12.601805, 0.821622, 0.683835, 0.088749, 0.00000095 },
    { 175.108530, -301.742502, 1.000000, 0.000000, 0.270426, 3.84041919 },
    { 72.306447, 47.257228, 1.000000, 0.000000, 0.077728, 38.99060546 },
    { 166.749585, -295.522751, 1.000000, 0.000000, 0.509112, 292.17079339 },
    { -44.144856, 102.626055, 0.900921, 0.159412, 0.372268, 0.00000096 },
    { -195.710234, 90.873036, 0.596979, 0.475909, 0.426437, 0.00001191 },
    { 276.205620, 189.150638, 1.000000, 0.528467, 0.000000, 46.44372051 },
    { -302.633860, -67.117545, 0.485602, 0.673778, 0.339062, 0.00000105 },
  };
#else

  DeltaTestData testData[] = {
    { -307.038493, 169.845569, 0.232411, 1.000000, 0.000000, 169.84556627 }
  };
#endif

  set_defaults();
  setup_indexing();

  settings.verbose = 0;
  settings.verbose_refinement = 0;
  settings.max_iters = 8;
  settings.better_start = 0;
  settings.debug = 0;
  settings.refine_steps = 0;

  J[0] = Vecf64<2>(400, 0); J[1] = Vecf64<2>(-200.0000, 346.4102); J[2] = Vecf64<2>(-200.0000, -346.4102);

  f64 qs[N_TURN_ACT];
  qs[0] = qs[1] = qs[2] = 1;

#ifdef __AVR_ATmega2560__
  u32 beginTime = millis();
#endif
  s32 nRepeat = 100;
  Vecf64<2> dx(testData[0].q_res[0], testData[0].q_res[1]);
  params.q[0] = qs[0];
  params.q[1] = qs[1];
  params.q[2] = qs[2];

  for (s32 j = 0; j < N_TURN_ACT; j++) {
    params.J[j * 2 + 0] = J[j].x;
    params.J[j * 2 + 1] = J[j].y;
  }

  params.dx[0] = dx.x;
  params.dx[1] = dx.y;
  for (s32 i = 0; i < nRepeat*sizeof(testData) / sizeof(testData[0]); i++) {
    //Vec2f64 dx(testData[i].q_res[0], testData[i].q_res[1]);

    solve();

#if 0
    Vec2f64 delta_x_actual = J[0] * vars.dq[0] + J[1] * vars.dq[1] + J[2] * vars.dq[2];
    f64 error = (delta_x_actual - dx).magnitude();
    f64 matlabError = testData[i].q_res[5];
    if (error > 1e-5 && ABS(error - matlabError) / matlabError > 1e-4)
      s32 y = 0;

#ifdef __AVR_ATmega2560__
    Serial.println("Error " + String(error) + " matlab error " + String(matlabError) + " difference " + String(ABS(error - matlabError)) + " dq 0 1 2 " + String(vars.dq[0]) + " " + String(vars.dq[1]) + " " + String(vars.dq[2]));
#endif

    qs[0] = testData[i].q_res[2];
    qs[1] = testData[i].q_res[3];
    qs[2] = testData[i].q_res[4];
#endif
  }


#ifdef __AVR_ATmega2560__
  u32 endTime = millis();
  f64 nTrials = (f64)(nRepeat*sizeof(testData) / sizeof(testData[0]));
  Serial.println("Average time " + String(((f64)endTime - beginTime) / nTrials));
#endif
}
#endif

#ifdef RUN_EKF_JACOBIAN_TEST
struct EKFJacobianTest
{
  f64 x_est[N_TURN_ACT + 1];
  f64 z[2];
  f64 dq[N_TURN_ACT];
};

void runTest()
{
  EKFJacobianTest testData[] = {
    { 396.871340, 393.709457, 409.240917, 0.009954, 8.813217, 14.139816, -0.985355, -1.000000, -0.976252 },
    { 399.121673, 396.748520, 411.205989, 0.077631, 29.463138, 71.165393, 0.064269, 0.065346, -0.023748 },
    { 399.694233, 394.383468, 400.645681, 0.239999, 7.236265, -102.446693, 0.024649, -0.065346, 0.254187 },
    { 393.383857, 382.193687, 396.758048, 0.141246, -136.779065, 76.965879, -0.103564, 0.401632, 0.110581 },
    { 395.126855, 372.575828, 392.548437, 0.051147, 82.209202, -82.543438, -0.000000, -0.378735, -0.121120 },
    { 395.882244, 391.321070, 389.662406, -0.068573, -286.118751, -14.070791, -0.000000, 0.637937, 0.756352 },
    { 394.992153, 395.598139, 380.748606, 0.039934, -60.993414, 30.389217, -0.000000, 0.168644, 0.000000 },
    { 395.072008, 378.080029, 394.772895, 0.174617, 180.678227, 37.695934, -0.000000, -0.489467, -0.443564 },
    { 394.772469, 381.919340, 389.421572, 0.226278, -38.691991, 6.459085, -0.000000, 0.082998, -0.002307 },
    { 394.299537, 386.421209, 356.145475, 0.198559, -84.341209, -72.000555, -0.000000, 0.169216, 0.408410 },
    { 393.504461, 405.119849, 345.464962, 0.457674, -153.091044, 58.602335, -0.000000, 0.385614, 0.037461 },
    { 393.508791, 409.169211, 349.359397, 0.455528, -31.603295, 55.488979, -0.000000, 0.022160, -0.017587 },
    { 393.701320, 410.635064, 379.244954, 0.497355, 2.886479, 208.676660, -0.000000, 0.000000, -0.523129 },
    { 393.701198, 410.616197, 379.077378, 0.495648, -4.784462, -5.278784, -0.000000, 0.000000, 0.021005 },
    { 393.696220, 402.577815, 363.681485, 0.423274, 118.402987, 18.169664, -0.000000, -0.357586, -0.314260 },
    { 393.520287, 432.085233, 364.136108, 0.613142, -158.600478, -0.081207, -0.000000, 0.357586, 0.139090 },
    { 399.079925, 425.713060, 352.517473, 0.513602, 56.119534, 122.147144, 0.125662, 0.000000, -0.305120 },
    { 356.272188, 423.053251, 287.187263, 0.545700, -20.749682, -76.560684, -0.125662, 0.000000, 0.334824 },
    { 352.349732, 423.223420, 295.455248, 0.542090, 2.158262, -53.586350, -0.000000, 0.000000, 0.135609 },
    { 367.616249, 423.080564, 264.315589, 0.590038, 10.158313, -61.204102, -0.000000, 0.000000, 0.303870 },
    { 378.415303, 406.162949, 239.896870, 0.663514, 58.898715, -53.352719, -0.000000, -0.146695, 0.225697 },
    { 382.723298, 369.174302, 245.312371, 0.847022, -46.739178, -41.133935, -0.000000, 0.146695, 0.000000 },
    { 399.189904, 369.579321, 179.632906, 1.058329, -72.958310, 60.342397, -0.000000, 0.000000, -0.566221 },
    { 401.868739, 370.375240, 158.426794, 1.068988, -5.084311, -3.055559, -0.000000, 0.000000, -0.210730 },
    { 403.721013, 371.113983, 136.432225, 1.060410, 49.369546, -84.988792, -0.000000, 0.000000, 0.776951 },
    { 508.156580, 371.537596, 150.102722, 1.034990, 64.758658, 371.815943, 0.528892, 0.000000, -1.000000 },
    { 459.304658, 371.266775, 161.662077, 1.028438, 78.029410, 139.263940, 0.392545, 0.000000, -0.000000 },
    { 456.208685, 299.847058, 161.736639, 1.057238, 109.691435, -78.277038, -0.230457, -0.569595, -0.000000 },
    { 422.006104, 301.433622, 167.035863, 1.048038, -23.852400, -107.624666, -0.347434, -0.139093, -0.000000 },
    { 425.106847, 298.994271, 167.034431, 0.994414, -159.865429, 94.730609, 0.216765, 0.708688, -0.000000 },
    { 422.119630, 281.170138, 167.526626, 0.950007, 90.870224, -190.754355, -0.503460, -0.800304, -0.000000 },
    { 416.731256, 317.134448, 175.854225, 0.847423, -226.131851, -146.355823, -0.056851, 0.800304, 1.000000 },
    { 404.042388, 352.639161, 200.284907, 0.697013, 197.454316, 138.605554, -0.000000, -0.603076, -0.992785 },
    { 412.203621, 359.693680, 184.045494, 0.820624, 199.264927, -205.221829, -0.000000, -0.396924, 0.992785 },
    { 412.204805, 359.607397, 184.059844, 0.821071, 2.122020, -7.700550, -0.000000, 0.004707, 0.000000 },
    { 411.709229, 359.671669, 184.034602, 0.818227, -0.138383, -17.089795, 0.003890, -0.004707, 0.000000 },
    { 411.476324, 359.668050, 184.038486, 0.818357, 4.068209, 2.875628, -0.003890, -0.000000, 0.000000 },
    { 411.476324, 359.668050, 184.038486, 0.818357, 21.543894, -32.608748, -0.000000, -0.000000, 0.000000 },
    { 368.182168, 361.411600, 142.475152, 0.786993, 77.965643, 201.385591, 0.412367, -0.000000, -0.760377 },
    { 371.513664, 361.539031, 137.642455, 0.792302, 13.387613, -10.538081, -0.008426, -0.000000, -0.065056 },
    { 307.137236, 339.816851, 167.638472, 0.507837, -364.009319, 111.048345, -0.403941, 0.830086, -0.040311 },
    { 303.725428, 339.309733, 177.327884, 0.526888, -45.810134, -120.874608, -0.000000, 0.169914, 0.830028 },
    { 303.918355, 339.168457, 177.168036, 0.524485, -7.790339, -3.789508, -0.000000, 0.000000, 0.035716 },
    { 303.918355, 339.168457, 177.168036, 0.524485, -11.767103, 40.503349, -0.000000, 0.000000, 0.000000 },
    { 311.621109, 305.011865, 175.282915, 0.608030, 184.844457, -79.174256, -0.000000, -0.684688, 0.000000 },
    { 311.469227, 304.607364, 175.204504, 0.629156, 32.422911, 5.249585, -0.000000, -0.095209, 0.000000 },
    { 309.640222, 313.807955, 173.795536, 0.737334, -241.771659, 79.598404, -0.000000, 0.779897, -0.050497 },
    { 309.416612, 313.682785, 180.658338, 0.740746, 13.505318, -25.763597, -0.000000, 0.000000, 0.050497 },
    { 310.264721, 313.335118, 148.078302, 0.804322, -22.811007, 65.483678, -0.000000, -0.036290, -0.550541 },
    { 309.996199, 313.956151, 160.985850, 0.793661, -31.999928, 81.031298, -0.000000, 0.036290, -0.449459 },
    { 309.996199, 313.956151, 160.985850, 0.793661, 6.340279, 16.557375, -0.000000, 0.000000, -0.000000 },
    { 309.996199, 313.956151, 160.985850, 0.793661, -5.575376, 11.445492, -0.000000, 0.000000, -0.000000 },
    { 297.326482, 314.058102, 161.014911, 0.794007, 111.854883, 113.823280, 0.541326, 0.000000, -0.000000 },
    { 242.876927, 314.420211, 161.048158, 0.744687, -83.647363, -65.459888, -0.541326, 0.000000, -0.000000 },
    { 230.085989, 314.529068, 160.968726, 0.573085, 142.479969, 62.999697, 0.670435, 0.000000, -0.000000 },
    { 224.527391, 314.538739, 160.975421, 0.586899, 53.134777, 40.251752, 0.329565, 0.000000, -0.000000 },
    { 218.606873, 330.101885, 160.924253, 0.660923, -15.328452, -62.357812, -0.308826, -0.072246, -0.000000 },
    { 216.552706, 347.192465, 160.869721, 0.749986, 103.093631, -35.087314, -0.076110, -0.317475, -0.000000 },
    { 208.702165, 364.588805, 160.820325, 0.803852, 64.143142, -36.831245, -0.153404, -0.194844, -0.000000 },
    { 210.547899, 372.686109, 160.802041, 0.859656, -17.752308, 53.080673, 0.196299, 0.071649, -0.000000 },
    //{ 243.116349, 440.651304, 160.626638, 0.701675, -353.848110, -33.861730, -0.657959, 0.512916, -0.000000 },
    //{ 243.140299, 440.636078, 161.490466, 0.701366, 2.469994, -7.226743, -0.000000, -0.004674, 0.025212 },
    //{ 252.871855, 430.982967, 136.865101, 0.525571, -22.661848, -131.543783, -0.000000, 0.004674, 0.939203 },
    //{ 236.688281, 399.148741, 130.904824, 0.491543, 307.904033, 23.664304, 0.402148, -0.682074, -0.964415 },
    //{ 213.105563, 391.963843, 128.244005, 0.538618, -69.792723, 144.584873, 0.597852, 0.522791, -0.000000 },
    //{ 213.299868, 391.886538, 128.308984, 0.536440, -23.379803, 13.450663, -0.010985, -0.002019, -0.000000 },
    //{ 141.040964, 398.761875, 143.826416, 0.295191, -71.155031, 16.234377, -0.548291, 0.006909, -0.000000 },
    //{ 118.137298, 400.427770, 146.978285, 0.221055, 60.652078, -7.754675, 0.559277, -0.019209, -0.000000 },
    //{ 118.006552, 407.340523, 147.825631, 0.173479, -42.692916, 65.991404, 0.000000, 0.173602, -0.000000 },
    //{ 118.005770, 407.071333, 147.857025, 0.168327, -38.514675, -18.947921, 0.000000, -0.003991, -0.000000 },
    //{ 118.003174, 410.083424, 147.675418, 0.213833, 42.009390, -0.361363, 0.000000, -0.035188, -0.000000 },
    //{ 98.827454, 409.258173, 108.799125, 0.217097, -115.001284, -134.046113, -1.000000, -0.040892, 1.000000 },
    //{ 98.049475, 403.883528, 109.496285, 0.211715, 2.894155, 4.201091, -0.000000, 0.040581, 0.000000 },
    //{ 103.804151, 402.563152, 88.075801, 0.176240, 94.022023, 100.444217, 0.700151, 0.039490, -1.000000 },
    //{ 111.821399, 426.630292, 83.825983, 0.212784, -10.443970, -81.621198, -0.541748, -0.164859, -0.000000 },
    //{ 139.424456, 404.459150, 79.606820, 0.169843, 60.478057, 58.732933, 0.599065, 0.164859, -0.000000 },
    //{ 150.226732, 406.779399, 77.426251, 0.178256, 49.956597, 1.106878, 0.209278, 0.000000, -0.000000 },
    //{ 133.238962, 382.035794, 87.526318, -0.060097, 53.689895, -132.729787, 0.033255, -0.371613, -0.000000 },
    //{ 136.682812, 346.190557, 84.021951, 0.124338, 144.024218, -159.431761, 0.000000, -0.628387, -0.000000 },
    //{ 130.385001, 346.061248, 166.067002, 0.046310, -96.407675, -151.770492, 0.000000, -0.000000, 1.000000 },
    //{ 172.277025, 346.338627, 134.769385, 0.128862, -49.037247, 60.638370, -0.400114, -0.000000, -0.692107 },
    //{ 167.541524, 330.688618, 128.331561, 0.150195, -55.916262, 86.008467, -0.134833, 0.253942, -0.307893 },
    //{ 150.921848, 329.644898, 108.094783, 0.118633, -61.837559, -74.514968, -0.465053, -0.121382, 0.476708 },
    //{ 119.438434, 332.573736, 132.260338, -0.006787, 52.973839, -114.035470, 0.612559, -0.132560, 0.523292 },
    //{ 154.569295, 327.534757, 137.373982, 0.019181, 95.023847, 17.703153, 0.387441, -0.000000, 0.000000 },
    //{ 161.699549, 275.294493, 122.373854, 0.345131, -339.458886, 164.221899, -1.000000, 0.805267, -0.659359 },
    //{ 164.027403, 272.450239, 170.274483, 0.345439, -26.528277, 127.966555, -0.000000, 0.194733, -0.340641 },
    //{ 168.891827, 261.562383, 208.018735, 0.312644, -36.201426, -224.465189, -0.000000, -0.071283, 1.000000 },
    //{ 171.951537, 259.887981, 206.430626, 0.185176, 86.692307, -129.302140, -0.000000, -0.585936, 0.000000 },
    //{ 173.992320, 241.635500, 197.537301, 0.239386, -91.084623, 151.008764, -0.000000, 0.635674, -0.282446 },
    //{ 173.746804, 242.848491, 184.117095, 0.257888, -11.897162, 23.152476, -0.000000, 0.021545, -0.199499 },
    //{ 139.122619, 239.621669, 202.995618, 0.425468, 82.626099, 155.652313, 0.632445, 0.000000, -0.518055 },
    //{ 142.350843, 248.387729, 209.879404, 0.410497, 26.767999, -204.500777, -0.632445, -0.591201, 0.358250 },
    //{ 142.888973, 247.363483, 217.627890, 0.437270, 65.806131, 17.415733, -0.000000, -0.250869, -0.194907 },
    //{ 143.776765, 236.964930, 223.060619, 0.479149, -171.666639, 4.916222, -0.000000, 0.842069, 0.442946 },
    //{ 164.730410, 232.732625, 215.165309, 0.339848, 189.871426, 179.040704, 1.000000, 0.000000, -0.606289 },
    //{ 151.625872, 283.332052, 236.115494, 0.221253, 189.627515, -207.889833, 0.000000, -0.921654, -0.000000 },
    //{ 150.891978, 282.010426, 237.167969, 0.204656, -168.798963, 191.683723, 0.000000, 0.908208, -0.000000 },
    //{ 151.781608, 281.732548, 235.864501, 0.270673, 142.249379, -126.199529, 0.000000, -0.671234, -0.000000 },
    //{ 149.713224, 285.556365, 235.976325, 0.344396, -66.435978, -73.982453, -0.679618, -0.107688, -0.000000 }
  };

  f64 strengths[3] = { 400.000000, 400.000000, 400.000000 };
  f64 thetas[3] = { 0.000000, 2.094395, 4.188790 };
  f64 E0[4] = { 0.100000, 0.100000, 0.100000, 0.100000 };
  f64 Q[2] = { 400.000000, 400.000000 };
  f64 R[4] = { 400.000000, 400.000000, 400.000000, 0.010000 };

  EKFJacobianEstimator estimator(Matrixf64<N_TURN_ACT + 1, N_TURN_ACT + 1>::Diagonal(R), Matrixf64<2, 2>::Diagonal(Q),
    Matrixf64<N_TURN_ACT + 1, N_TURN_ACT + 1>::Diagonal(E0), Vecf64<N_TURN_ACT>(thetas), Vecf64<N_TURN_ACT>(strengths));

  for (s32 n = 0; n < sizeof(testData) / sizeof(testData[0]); n++) {
    Vecf64<N_TURN_ACT+1> xest = estimator.UpdateState(Vecf64<2>(testData[n].z), Vecf64<N_TURN_ACT>(testData[n].dq));

    f64 maxError = -10000;
    for (s32 ii = 0; ii < N_TURN_ACT + 1; ii++) {
      maxError = MAX(maxError, ABS(xest(ii) - testData[n].x_est[ii]));
    }

#ifdef _WIN32
    char debugStr[100] = { 0 };
    sprintf(debugStr, "MaxError %f\n", maxError);
    OutputDebugString(debugStr);
#else
    Serial.println("Maxerror " + String(maxError,10));
#endif
  }
}
#endif

#ifdef RUN_KF_JACOBIAN_TEST
struct KFJacobianTest
{
  f64 x_est[2*N_TURN_ACT];
  f64 z[2];
  f64 dq[N_TURN_ACT];
};

void runTest()
{
  KFJacobianTest testData[] = { 
  { 3.002441, 0.002114, -1.496994, 2.600680, -1.497499, -2.595910, 0.305152, -0.442646, -0.811917, -1.000000, -0.831983 },
  { 2.940070, 0.044153, -1.443439, 2.564583, -1.505476, -2.590534, 0.668113, -0.378211, 0.304818, -0.000000, 0.151278 },
  { 2.990307, -0.078936, -1.412230, 2.488117, -1.587533, -2.389482, 1.137440, -0.057097, 0.385760, -0.000000, 0.010526 },
  { 2.990357, -0.079002, -1.487706, 2.586550, -1.491985, -2.514092, -0.543617, 1.608804, -0.199344, 0.294821, -0.329821 },
  { 2.923771, -0.063182, -1.540132, 2.599005, -1.500966, -2.511958, 0.129231, 0.086656, 0.066772, 0.033673, -0.000000 },
  { 2.946957, 0.063613, -1.551936, 2.534457, -1.510553, -2.564384, 0.873105, -0.179962, 0.253912, -0.079736, -0.000000 },
  { 2.942385, 0.054954, -1.600527, 2.442440, -1.524358, -2.590528, -0.211119, 0.195694, -0.024330, 0.084274, -0.000000 },
  { 2.934070, 0.058266, -1.675998, 2.472500, -1.539326, -2.584566, 0.417202, -0.504774, 0.024330, -0.204135, -0.000000 },
  { 2.922539, 0.056002, -1.727184, 2.462448, -1.543653, -2.585416, -0.123842, 0.150430, 0.000000, 0.062374, -0.000000 },
  { 2.923329, 0.055050, -1.721205, 2.455243, -1.543357, -2.585773, 0.067633, -0.096928, 0.000000, -0.040478, -0.000000 },
  { 2.913831, 0.045305, -1.826404, 2.347301, -1.546921, -2.589431, 0.282768, -0.346408, 0.000000, -0.150794, -0.000000 },
  { 2.913831, 0.045305, -1.826404, 2.347301, -1.546921, -2.589431, -0.007090, 0.000412, 0.000000, -0.000000, -0.000000 },
  { 2.916519, 0.228102, -1.826385, 2.348585, -1.551104, -2.873860, -2.521534, -1.871078, -0.541437, -0.000000, 0.607568 },
  { 3.184064, 0.342273, -1.968523, 2.287929, -1.243128, -2.742436, 0.115970, 2.454915, 0.012685, 0.343485, -0.607568 },
  { 3.133799, 0.283037, -2.040220, 2.203435, -1.260861, -2.763334, -0.010732, -1.381233, -0.142273, -0.343485, 0.212175 },
  { 3.196766, 0.353116, -2.016901, 2.229388, -1.166699, -2.658537, 0.050035, -0.336591, 0.063797, -0.000000, 0.138100 },
  { 3.042681, 0.399208, -1.989040, 2.221054, -1.172163, -2.656902, 0.201988, -0.037495, 0.083147, -0.000000, 0.028577 },
  { 2.980706, 0.240927, -1.980153, 2.243750, -1.172332, -2.657334, 1.427062, -0.180263, 0.524081, -0.000000, 0.114135 },
  { 2.973540, 0.242358, -1.972482, 2.242218, -1.120979, -2.667588, 0.406399, 1.291302, -0.047123, -0.000000, -0.488289 },
  { 2.969943, 0.215273, -2.010207, 1.958179, -1.120699, -2.665480, -0.191427, 0.339529, 0.047123, 0.166820, -0.004698 },
  { 2.953096, 0.219570, -2.025920, 1.962186, -1.120369, -2.665565, 0.027330, -0.029436, -0.007769, -0.010314, -0.000000 },
  { 2.924196, 0.213979, -1.832737, 1.999557, -1.116614, -2.664838, -0.892966, 1.006010, 0.007769, 0.501885, -0.000000 },
  { 2.996916, 0.149589, -1.813867, 1.982849, -1.118319, -2.663329, -0.365736, -1.177894, -0.460485, -0.559774, -0.000000 },
  { 2.999562, 0.113959, -1.815810, 2.009001, -1.090657, -3.035791, -0.331097, -2.894207, 0.154557, -0.098616, 0.893011 },
  { 2.953313, 0.259545, -1.802994, 1.968659, -1.089162, -3.040494, 0.785375, -0.240871, 0.305928, -0.000000, 0.106989 },
  { 2.973587, 0.290683, -1.819966, 1.942595, -1.194107, -3.201665, 0.565176, 1.511432, 0.000000, -0.000000, -0.470431 },
  { 2.930483, 0.230263, -1.817458, 1.946110, -1.202930, -3.214032, -1.111211, 0.570196, -0.465927, -0.000000, -0.210224 },
  { 2.885948, 0.258045, -1.817052, 1.945857, -1.216540, -3.205542, 1.120332, -0.461863, 0.465927, -0.000000, 0.182124 },
  { 2.875300, 0.237859, -1.816326, 1.947233, -1.173354, -3.123670, 0.317365, 0.846247, 0.000000, -0.000000, -0.271997 },
  { 2.869710, 0.241831, -1.816183, 1.947132, -1.146574, -3.142697, -0.034115, -0.128854, -0.003202, -0.000000, 0.036841 },
  };

  f64 J0[2][3] = { { 3.000000, -1.500000, -1.500000 },{ 0.000000, 2.598076, -2.598076 } };
  f64 E0[6] = { 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000 };
  f64 Q[2] = { 0.000100, 0.000100 };
  f64 R[6] = { 0.003000, 0.003000, 0.003000, 0.003000, 0.003000, 0.003000 };

  J = Matrixf64<2, N_TURN_ACT>(J0);

  KFJacobianEstimator estimator;
  estimator.Initialize(J,Matrixf64<2*N_TURN_ACT,2*N_TURN_ACT>::Diagonal(R), Matrixf64<2,2>::Diagonal(Q),
    Matrixf64<2 * N_TURN_ACT, 2 * N_TURN_ACT>::Diagonal(E0));

  estimator.SetAlpha(1.0);

  for (s32 n = 0; n < sizeof(testData) / sizeof(testData[0]); n++) {
    Matrixf64<2,N_TURN_ACT> xx = estimator.Update(Vecf64<2>(testData[n].z), Vecf64<N_TURN_ACT>(testData[n].dq));
    Vecf64<2 * N_TURN_ACT> xest = flattenMatrix(xx);

    f64 maxError = -10000;
    for (s32 ii = 0; ii < N_TURN_ACT + 1; ii++) {
      maxError = MAX(maxError, ABS(xest(ii) - testData[n].x_est[ii]));
    }

    estimator.SetJacobian(unflattenVector(Vecf64<2 * N_TURN_ACT>(&testData[n].x_est[0])));

#ifdef _WIN32
    char debugStr[100] = { 0 };
    sprintf(debugStr, "%d MaxError %f\n", n, maxError);
    OutputDebugString(debugStr);
#else
    Serial.println("Maxerror " + String(maxError, 10));
#endif
  }
}
#endif

#ifdef RUN_BVLS_TEST

Vecf64<3> runBVLS(Matrixf64<2, 3> J, Vecf64<3> l, Vecf64<3> u, Vecf64<2> dx);

void runTest()
{
  f64 dx[] = { -0.0166616480, -0.0663095940 };
  f64 l[] = { -0.0000000000, -0.0000000000, -0.0000000000 };
  f64 u[] = { 1.0000000000, 1.0000000000, 1.0000000000 };
  f64 J[2][3] = { {-2.580425, -0.738108, 6.272839},
  {-5.359344, 2.616928, 1.000985}
  };

  Matrixf64<2, 3> JJ(J);
  Vecf64<2> ddx(dx);
  Vecf64<3> ll(l);
  Vecf64<3> uu(u);

  Vecf64<3> dq = runBVLS(JJ, ll, uu, ddx);

  int x = 0;
}
#endif

#ifndef __AVR_ATmega2560__
int main(int argc, char **argv)
{
  runTest();
}
#endif