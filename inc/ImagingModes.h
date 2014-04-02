#pragma once

#include <string>

// Do not edit this file.
// This file is automatically generated from modes.xml.
// Any changes will be lost.
//
enum imagingMode
{
    UnknownMode = -1,
    StartMode = 0,

    BMode = 0,
    MMode = 1,
    ColourMode = 2,
    PwMode = 3,
    TriplexMode = 4,
    PanoMode = 5,
    DualMode = 6,
    QuadMode = 7,
    CompoundMode = 8,
    DualColourMode = 9,
    DualCompoundMode = 10,
    CwMode = 11,
    ColorSplitMode = 12,
    F4DMode = 13,
    TriplexCwMode = 14,
    ColourMMode = 15,
    ElastoMode = 16,
    AnatomicalMMode = 17,
    ElastoComparativeMode = 18,
    VecDopMode = 19,
    BiplaneMode = 20,
    FibroMode = 21,
    Reslice3DMode = 22,
    OAMode = 23,
    ContrastMode = 24,
    OAHexMode = 25,
    OAQuadMode = 26,

    EndMode // the last one 
};


inline std::wstring imagingModeToString(imagingMode mode)
{
    switch (mode)
    {
    case BMode: return L"B";
    case MMode: return L"M";
    case ColourMode: return L"Color Doppler";
    case PwMode: return L"Pulsed Wave Doppler";
    case TriplexMode: return L"Triplex";
    case PanoMode: return L"Panoramic";
    case DualMode: return L"Dual B";
    case QuadMode: return L"Quad B";
    case CompoundMode: return L"Compound";
    case DualColourMode: return L"Dual Color";
    case DualCompoundMode: return L"Dual Compound";
    case CwMode: return L"Continuous Wave Doppler";
    case ColorSplitMode: return L"Color Split";
    case F4DMode: return L"4D";
    case TriplexCwMode: return L"Triplex CW";
    case ColourMMode: return L"Color M";
    case ElastoMode: return L"Elastography";
    case AnatomicalMMode: return L"Anatomical M";
    case ElastoComparativeMode: return L"Elastography Comparative";
    case VecDopMode: return L"Vector Doppler";
    case BiplaneMode: return L"Biplane";
    case FibroMode: return L"Fibro";
    case Reslice3DMode: return L"Reslice 3D";
    case OAMode: return L"OA";
    case ContrastMode: return L"Contrast";
    case OAHexMode: return L"Hex OA";
    case OAQuadMode: return L"Quad OA";
    default: return std::wstring();
    }
}
