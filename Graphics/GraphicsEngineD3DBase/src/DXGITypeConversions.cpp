/*
 *  Copyright 2019-2025 Diligent Graphics LLC
 *  Copyright 2015-2019 Egor Yusov
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  In no event and under no legal theory, whether in tort (including negligence),
 *  contract, or otherwise, unless required by applicable law (such as deliberate
 *  and grossly negligent acts) or agreed to in writing, shall any Contributor be
 *  liable for any damages, including any direct, indirect, special, incidental,
 *  or consequential damages of any character arising as a result of this License or
 *  out of the use or inability to use the software (including but not limited to damages
 *  for loss of goodwill, work stoppage, computer failure or malfunction, or any and
 *  all other commercial damages or losses), even if such Contributor has been advised
 *  of the possibility of such damages.
 */

#include "DXGITypeConversions.hpp"
#include "BasicTypes.h"
#include "DebugUtilities.hpp"
#include "GraphicsAccessories.hpp"

namespace Diligent
{

DXGI_COLOR_SPACE_TYPE ColorSpaceToDXGIColorSpace(COLOR_SPACE ColorSpace)
{
    switch (ColorSpace)
    {
        case COLOR_SPACE_SRGB_NONLINEAR:
            return DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709;

        case COLOR_SPACE_EXTENDED_SRGB_LINEAR:
            return DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709;

        case COLOR_SPACE_EXTENDED_SRGB_NONLINEAR:
            // DXGI doesn't distinguish extended vs non-extended for nonlinear
            // The "extended" behavior comes from using a float format
            return DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709;

        case COLOR_SPACE_DISPLAY_P3_NONLINEAR:
            // DXGI has no Display-P3 primaries
            // Fall back to sRGB (same transfer function, narrower gamut)
            return DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709;

        case COLOR_SPACE_DISPLAY_P3_LINEAR:
            // DXGI has no Display-P3 primaries
            // Fall back to linear sRGB
            return DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709;

        case COLOR_SPACE_DCI_P3_NONLINEAR:
            // DXGI has no DCI-P3 (gamma 2.6)
            // Fall back to sRGB
            return DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709;

        case COLOR_SPACE_BT709_LINEAR:
            // BT.709 primaries are the same as sRGB
            return DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709;

        case COLOR_SPACE_BT709_NONLINEAR:
            // BT.709 transfer function is very close to sRGB
            return DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709;

        case COLOR_SPACE_BT2020_LINEAR:
            // DXGI has no linear BT.2020 for full range RGB
            // Use HDR10 PQ as closest wide-gamut alternative
            return DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020;

        case COLOR_SPACE_HDR10_ST2084:
            return DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020;

        case COLOR_SPACE_HDR10_HLG:
            // DXGI has no direct HLG for RGB
            // G22 with P2020 primaries is the closest approximation
            return DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P2020;

        case COLOR_SPACE_DOLBY_VISION:
            // Dolby Vision not directly supported in DXGI
            // Fall back to HDR10 PQ
            return DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020;

        case COLOR_SPACE_ADOBE_RGB_NONLINEAR:
            // DXGI has no Adobe RGB primaries
            // Fall back to sRGB (similar gamma, narrower gamut)
            return DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709;

        case COLOR_SPACE_ADOBE_RGB_LINEAR:
            // DXGI has no Adobe RGB primaries
            // Fall back to linear sRGB
            return DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709;

        case COLOR_SPACE_PASS_THROUGH:
            // No transformation - use sRGB as neutral default
            return DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709;

        case COLOR_SPACE_SCRGB_LINEAR:
            // scRGB is linear with BT.709/sRGB primaries, extended range via float format
            return DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709;

        case COLOR_SPACE_UNKNOWN:
        default:
            return DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709;
    }
}

COLOR_SPACE DXGIColorSpaceToColorSpace(DXGI_COLOR_SPACE_TYPE DXGIColorSpace)
{
    switch (DXGIColorSpace)
    {
        // Full range RGB formats
        case DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709:
            return COLOR_SPACE_SRGB_NONLINEAR;

        case DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709:
            // Could be EXTENDED_SRGB_LINEAR, BT709_LINEAR, or SCRGB_LINEAR
            // They all map to the same DXGI value; choose scRGB as it's most descriptive for HDR
            return COLOR_SPACE_SCRGB_LINEAR;

        case DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020:
            return COLOR_SPACE_HDR10_ST2084;

        case DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P2020:
            // BT.2020 primaries with gamma 2.2
            // Closest to HLG conceptually (wide gamut, SDR-compatible transfer)
            return COLOR_SPACE_HDR10_HLG;

        // Studio range RGB formats (limited range 16-235)
        case DXGI_COLOR_SPACE_RGB_STUDIO_G22_NONE_P709:
            // Studio range sRGB - map to regular sRGB (application should handle range)
            return COLOR_SPACE_SRGB_NONLINEAR;

        case DXGI_COLOR_SPACE_RGB_STUDIO_G22_NONE_P2020:
            // Studio range BT.2020 with gamma 2.2
            return COLOR_SPACE_HDR10_HLG;

        case DXGI_COLOR_SPACE_RGB_STUDIO_G2084_NONE_P2020:
            // Studio range HDR10
            return COLOR_SPACE_HDR10_ST2084;

        case DXGI_COLOR_SPACE_RGB_STUDIO_G24_NONE_P709:
            // Studio range, gamma 2.4, BT.709 - used for some broadcast
            return COLOR_SPACE_BT709_NONLINEAR;

        case DXGI_COLOR_SPACE_RGB_STUDIO_G24_NONE_P2020:
            // Studio range, gamma 2.4, BT.2020
            return COLOR_SPACE_HDR10_HLG;

        // YCbCr formats - these are typically for video, not swap chains
        // Map them to reasonable RGB equivalents based on primaries..
        case DXGI_COLOR_SPACE_YCBCR_FULL_G22_NONE_P709_X601:
        case DXGI_COLOR_SPACE_YCBCR_STUDIO_G22_LEFT_P601:
        case DXGI_COLOR_SPACE_YCBCR_FULL_G22_LEFT_P601:
        case DXGI_COLOR_SPACE_YCBCR_STUDIO_G22_LEFT_P709:
        case DXGI_COLOR_SPACE_YCBCR_FULL_G22_LEFT_P709:
        case DXGI_COLOR_SPACE_YCBCR_STUDIO_G24_LEFT_P709:
            // BT.601/709 YCbCr -> sRGB
            return COLOR_SPACE_SRGB_NONLINEAR;

        case DXGI_COLOR_SPACE_YCBCR_STUDIO_G22_LEFT_P2020:
        case DXGI_COLOR_SPACE_YCBCR_FULL_G22_LEFT_P2020:
        case DXGI_COLOR_SPACE_YCBCR_STUDIO_G22_TOPLEFT_P2020:
        case DXGI_COLOR_SPACE_YCBCR_STUDIO_G24_LEFT_P2020:
        case DXGI_COLOR_SPACE_YCBCR_STUDIO_G24_TOPLEFT_P2020:
            // BT.2020 YCbCr with gamma -> HLG-ish
            return COLOR_SPACE_HDR10_HLG;

        case DXGI_COLOR_SPACE_YCBCR_STUDIO_G2084_LEFT_P2020:
        case DXGI_COLOR_SPACE_YCBCR_STUDIO_G2084_TOPLEFT_P2020:
            // BT.2020 YCbCr with PQ -> HDR10
            return COLOR_SPACE_HDR10_ST2084;

        case DXGI_COLOR_SPACE_YCBCR_STUDIO_GHLG_TOPLEFT_P2020:
        case DXGI_COLOR_SPACE_YCBCR_FULL_GHLG_TOPLEFT_P2020:
            // Actual HLG YCbCr
            return COLOR_SPACE_HDR10_HLG;

        case DXGI_COLOR_SPACE_RESERVED:
        case DXGI_COLOR_SPACE_CUSTOM:
        default:
            return COLOR_SPACE_SRGB_NONLINEAR;
    }
}

DXGI_FORMAT TypeToDXGI_Format(VALUE_TYPE ValType, Uint32 NumComponents, Bool bIsNormalized)
{
    switch (ValType)
    {
        case VT_FLOAT16:
        {
            VERIFY(!bIsNormalized, "Floating point formats cannot be normalized");
            switch (NumComponents)
            {
                case 1: return DXGI_FORMAT_R16_FLOAT;
                case 2: return DXGI_FORMAT_R16G16_FLOAT;
                case 4: return DXGI_FORMAT_R16G16B16A16_FLOAT;
                default: UNEXPECTED("Unsupported number of components"); return DXGI_FORMAT_UNKNOWN;
            }
        }

        case VT_FLOAT32:
        {
            VERIFY(!bIsNormalized, "Floating point formats cannot be normalized");
            switch (NumComponents)
            {
                case 1: return DXGI_FORMAT_R32_FLOAT;
                case 2: return DXGI_FORMAT_R32G32_FLOAT;
                case 3: return DXGI_FORMAT_R32G32B32_FLOAT;
                case 4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
                default: UNEXPECTED("Unsupported number of components"); return DXGI_FORMAT_UNKNOWN;
            }
        }

        case VT_INT32:
        {
            VERIFY(!bIsNormalized, "32-bit UNORM formats are not supported. Use R32_FLOAT instead");
            switch (NumComponents)
            {
                case 1: return DXGI_FORMAT_R32_SINT;
                case 2: return DXGI_FORMAT_R32G32_SINT;
                case 3: return DXGI_FORMAT_R32G32B32_SINT;
                case 4: return DXGI_FORMAT_R32G32B32A32_SINT;
                default: UNEXPECTED("Unsupported number of components"); return DXGI_FORMAT_UNKNOWN;
            }
        }

        case VT_UINT32:
        {
            VERIFY(!bIsNormalized, "32-bit UNORM formats are not supported. Use R32_FLOAT instead");
            switch (NumComponents)
            {
                case 1: return DXGI_FORMAT_R32_UINT;
                case 2: return DXGI_FORMAT_R32G32_UINT;
                case 3: return DXGI_FORMAT_R32G32B32_UINT;
                case 4: return DXGI_FORMAT_R32G32B32A32_UINT;
                default: UNEXPECTED("Unsupported number of components"); return DXGI_FORMAT_UNKNOWN;
            }
        }

        case VT_INT16:
        {
            if (bIsNormalized)
            {
                switch (NumComponents)
                {
                    case 1: return DXGI_FORMAT_R16_SNORM;
                    case 2: return DXGI_FORMAT_R16G16_SNORM;
                    case 4: return DXGI_FORMAT_R16G16B16A16_SNORM;
                    default: UNEXPECTED("Unsupported number of components"); return DXGI_FORMAT_UNKNOWN;
                }
            }
            else
            {
                switch (NumComponents)
                {
                    case 1: return DXGI_FORMAT_R16_SINT;
                    case 2: return DXGI_FORMAT_R16G16_SINT;
                    case 4: return DXGI_FORMAT_R16G16B16A16_SINT;
                    default: UNEXPECTED("Unsupported number of components"); return DXGI_FORMAT_UNKNOWN;
                }
            }
        }

        case VT_UINT16:
        {
            if (bIsNormalized)
            {
                switch (NumComponents)
                {
                    case 1: return DXGI_FORMAT_R16_UNORM;
                    case 2: return DXGI_FORMAT_R16G16_UNORM;
                    case 4: return DXGI_FORMAT_R16G16B16A16_UNORM;
                    default: UNEXPECTED("Unsupported number of components"); return DXGI_FORMAT_UNKNOWN;
                }
            }
            else
            {
                switch (NumComponents)
                {
                    case 1: return DXGI_FORMAT_R16_UINT;
                    case 2: return DXGI_FORMAT_R16G16_UINT;
                    case 4: return DXGI_FORMAT_R16G16B16A16_UINT;
                    default: UNEXPECTED("Unsupported number of components"); return DXGI_FORMAT_UNKNOWN;
                }
            }
        }

        case VT_INT8:
        {
            if (bIsNormalized)
            {
                switch (NumComponents)
                {
                    case 1: return DXGI_FORMAT_R8_SNORM;
                    case 2: return DXGI_FORMAT_R8G8_SNORM;
                    case 4: return DXGI_FORMAT_R8G8B8A8_SNORM;
                    default: UNEXPECTED("Unsupported number of components"); return DXGI_FORMAT_UNKNOWN;
                }
            }
            else
            {
                switch (NumComponents)
                {
                    case 1: return DXGI_FORMAT_R8_SINT;
                    case 2: return DXGI_FORMAT_R8G8_SINT;
                    case 4: return DXGI_FORMAT_R8G8B8A8_SINT;
                    default: UNEXPECTED("Unsupported number of components"); return DXGI_FORMAT_UNKNOWN;
                }
            }
        }

        case VT_UINT8:
        {
            if (bIsNormalized)
            {
                switch (NumComponents)
                {
                    case 1: return DXGI_FORMAT_R8_UNORM;
                    case 2: return DXGI_FORMAT_R8G8_UNORM;
                    case 4: return DXGI_FORMAT_R8G8B8A8_UNORM;
                    default: UNEXPECTED("Unsupported number of components"); return DXGI_FORMAT_UNKNOWN;
                }
            }
            else
            {
                switch (NumComponents)
                {
                    case 1: return DXGI_FORMAT_R8_UINT;
                    case 2: return DXGI_FORMAT_R8G8_UINT;
                    case 4: return DXGI_FORMAT_R8G8B8A8_UINT;
                    default: UNEXPECTED("Unsupported number of components"); return DXGI_FORMAT_UNKNOWN;
                }
            }
        }

        default: UNEXPECTED("Unsupported format"); return DXGI_FORMAT_UNKNOWN;
    }
}

DXGI_FORMAT CorrectDXGIFormat(DXGI_FORMAT DXGIFormat, Uint32 BindFlags)
{
    if ((BindFlags & BIND_DEPTH_STENCIL) && (BindFlags != BIND_DEPTH_STENCIL))
    {
        switch (DXGIFormat)
        {
            case DXGI_FORMAT_R32_TYPELESS:
            case DXGI_FORMAT_R32_FLOAT:
            case DXGI_FORMAT_D32_FLOAT:
                DXGIFormat = DXGI_FORMAT_R32_TYPELESS;
                break;

            case DXGI_FORMAT_R24G8_TYPELESS:
            case DXGI_FORMAT_D24_UNORM_S8_UINT:
            case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
            case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
                DXGIFormat = DXGI_FORMAT_R24G8_TYPELESS;
                break;

            case DXGI_FORMAT_R16_TYPELESS:
            case DXGI_FORMAT_R16_UNORM:
            case DXGI_FORMAT_D16_UNORM:
                DXGIFormat = DXGI_FORMAT_R16_TYPELESS;
                break;

            case DXGI_FORMAT_R32G8X24_TYPELESS:
            case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
            case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
            case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
                DXGIFormat = DXGI_FORMAT_R32G8X24_TYPELESS;
                break;

            default:
                UNEXPECTED("Unsupported depth-stencil format");
                break;
        }
    }

    if (BindFlags == BIND_DEPTH_STENCIL)
    {
        switch (DXGIFormat)
        {
            case DXGI_FORMAT_R32_TYPELESS:
            case DXGI_FORMAT_R32_FLOAT:
                DXGIFormat = DXGI_FORMAT_D32_FLOAT;
                break;

            case DXGI_FORMAT_R24G8_TYPELESS:
            case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
            case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
                DXGIFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
                break;

            case DXGI_FORMAT_R16_TYPELESS:
            case DXGI_FORMAT_R16_UNORM:
                DXGIFormat = DXGI_FORMAT_D16_UNORM;
                break;

            case DXGI_FORMAT_R32G8X24_TYPELESS:
            case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
            case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
                DXGIFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
                break;

            default:
                break;
        }
    }

    if (BindFlags == BIND_SHADER_RESOURCE || BindFlags == BIND_UNORDERED_ACCESS)
    {
        switch (DXGIFormat)
        {
            case DXGI_FORMAT_R32_TYPELESS:
            case DXGI_FORMAT_D32_FLOAT:
                DXGIFormat = DXGI_FORMAT_R32_FLOAT;
                break;

            case DXGI_FORMAT_R24G8_TYPELESS:
            case DXGI_FORMAT_D24_UNORM_S8_UINT:
                DXGIFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
                break;

            case DXGI_FORMAT_R16_TYPELESS:
            case DXGI_FORMAT_D16_UNORM:
                DXGIFormat = DXGI_FORMAT_R16_UNORM;
                break;

            case DXGI_FORMAT_R32G8X24_TYPELESS:
            case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
                DXGIFormat = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
                break;

            default:
                break;
        }
    }

    return DXGIFormat;
}

DXGI_FORMAT TexFormatToDXGI_Format(TEXTURE_FORMAT TexFormat, Uint32 BindFlags)
{
    static Bool        bFormatMapInitialized                   = false;
    static DXGI_FORMAT FmtToDXGIFmtMap[TEX_FORMAT_NUM_FORMATS] = {DXGI_FORMAT_UNKNOWN};
    if (!bFormatMapInitialized)
    {
        // clang-format off
        FmtToDXGIFmtMap[TEX_FORMAT_UNKNOWN]                = DXGI_FORMAT_UNKNOWN;

        FmtToDXGIFmtMap[TEX_FORMAT_RGBA32_TYPELESS]        = DXGI_FORMAT_R32G32B32A32_TYPELESS;
        FmtToDXGIFmtMap[TEX_FORMAT_RGBA32_FLOAT]           = DXGI_FORMAT_R32G32B32A32_FLOAT;
        FmtToDXGIFmtMap[TEX_FORMAT_RGBA32_UINT]            = DXGI_FORMAT_R32G32B32A32_UINT;
        FmtToDXGIFmtMap[TEX_FORMAT_RGBA32_SINT]            = DXGI_FORMAT_R32G32B32A32_SINT;

        FmtToDXGIFmtMap[TEX_FORMAT_RGB32_TYPELESS]         = DXGI_FORMAT_R32G32B32_TYPELESS;
        FmtToDXGIFmtMap[TEX_FORMAT_RGB32_FLOAT]            = DXGI_FORMAT_R32G32B32_FLOAT;
        FmtToDXGIFmtMap[TEX_FORMAT_RGB32_UINT]             = DXGI_FORMAT_R32G32B32_UINT;
        FmtToDXGIFmtMap[TEX_FORMAT_RGB32_SINT]             = DXGI_FORMAT_R32G32B32_SINT;

        FmtToDXGIFmtMap[TEX_FORMAT_RGBA16_TYPELESS]        = DXGI_FORMAT_R16G16B16A16_TYPELESS;
        FmtToDXGIFmtMap[TEX_FORMAT_RGBA16_FLOAT]           = DXGI_FORMAT_R16G16B16A16_FLOAT;
        FmtToDXGIFmtMap[TEX_FORMAT_RGBA16_UNORM]           = DXGI_FORMAT_R16G16B16A16_UNORM;
        FmtToDXGIFmtMap[TEX_FORMAT_RGBA16_UINT]            = DXGI_FORMAT_R16G16B16A16_UINT;
        FmtToDXGIFmtMap[TEX_FORMAT_RGBA16_SNORM]           = DXGI_FORMAT_R16G16B16A16_SNORM;
        FmtToDXGIFmtMap[TEX_FORMAT_RGBA16_SINT]            = DXGI_FORMAT_R16G16B16A16_SINT;

        FmtToDXGIFmtMap[TEX_FORMAT_RG32_TYPELESS]          = DXGI_FORMAT_R32G32_TYPELESS;
        FmtToDXGIFmtMap[TEX_FORMAT_RG32_FLOAT]             = DXGI_FORMAT_R32G32_FLOAT;
        FmtToDXGIFmtMap[TEX_FORMAT_RG32_UINT]              = DXGI_FORMAT_R32G32_UINT;
        FmtToDXGIFmtMap[TEX_FORMAT_RG32_SINT]              = DXGI_FORMAT_R32G32_SINT;

        FmtToDXGIFmtMap[TEX_FORMAT_R32G8X24_TYPELESS]      = DXGI_FORMAT_R32G8X24_TYPELESS;
        FmtToDXGIFmtMap[TEX_FORMAT_D32_FLOAT_S8X24_UINT]   = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        FmtToDXGIFmtMap[TEX_FORMAT_R32_FLOAT_X8X24_TYPELESS]= DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
        FmtToDXGIFmtMap[TEX_FORMAT_X32_TYPELESS_G8X24_UINT]= DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;

        FmtToDXGIFmtMap[TEX_FORMAT_RGB10A2_TYPELESS]       = DXGI_FORMAT_R10G10B10A2_TYPELESS;
        FmtToDXGIFmtMap[TEX_FORMAT_RGB10A2_UNORM]          = DXGI_FORMAT_R10G10B10A2_UNORM;
        FmtToDXGIFmtMap[TEX_FORMAT_RGB10A2_UINT]           = DXGI_FORMAT_R10G10B10A2_UINT;

        FmtToDXGIFmtMap[TEX_FORMAT_R11G11B10_FLOAT]        = DXGI_FORMAT_R11G11B10_FLOAT;

        FmtToDXGIFmtMap[TEX_FORMAT_RGBA8_TYPELESS]         = DXGI_FORMAT_R8G8B8A8_TYPELESS;
        FmtToDXGIFmtMap[TEX_FORMAT_RGBA8_UNORM]            = DXGI_FORMAT_R8G8B8A8_UNORM;
        FmtToDXGIFmtMap[TEX_FORMAT_RGBA8_UNORM_SRGB]       = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        FmtToDXGIFmtMap[TEX_FORMAT_RGBA8_UINT]             = DXGI_FORMAT_R8G8B8A8_UINT;
        FmtToDXGIFmtMap[TEX_FORMAT_RGBA8_SNORM]            = DXGI_FORMAT_R8G8B8A8_SNORM;
        FmtToDXGIFmtMap[TEX_FORMAT_RGBA8_SINT]             = DXGI_FORMAT_R8G8B8A8_SINT;

        FmtToDXGIFmtMap[TEX_FORMAT_RG16_TYPELESS]          = DXGI_FORMAT_R16G16_TYPELESS;
        FmtToDXGIFmtMap[TEX_FORMAT_RG16_FLOAT]             = DXGI_FORMAT_R16G16_FLOAT;
        FmtToDXGIFmtMap[TEX_FORMAT_RG16_UNORM]             = DXGI_FORMAT_R16G16_UNORM;
        FmtToDXGIFmtMap[TEX_FORMAT_RG16_UINT]              = DXGI_FORMAT_R16G16_UINT;
        FmtToDXGIFmtMap[TEX_FORMAT_RG16_SNORM]             = DXGI_FORMAT_R16G16_SNORM;
        FmtToDXGIFmtMap[TEX_FORMAT_RG16_SINT]              = DXGI_FORMAT_R16G16_SINT;

        FmtToDXGIFmtMap[TEX_FORMAT_R32_TYPELESS]           = DXGI_FORMAT_R32_TYPELESS;
        FmtToDXGIFmtMap[TEX_FORMAT_D32_FLOAT]              = DXGI_FORMAT_D32_FLOAT;
        FmtToDXGIFmtMap[TEX_FORMAT_R32_FLOAT]              = DXGI_FORMAT_R32_FLOAT;
        FmtToDXGIFmtMap[TEX_FORMAT_R32_UINT]               = DXGI_FORMAT_R32_UINT;
        FmtToDXGIFmtMap[TEX_FORMAT_R32_SINT]               = DXGI_FORMAT_R32_SINT;

        FmtToDXGIFmtMap[TEX_FORMAT_R24G8_TYPELESS]         = DXGI_FORMAT_R24G8_TYPELESS;
        FmtToDXGIFmtMap[TEX_FORMAT_D24_UNORM_S8_UINT]      = DXGI_FORMAT_D24_UNORM_S8_UINT;
        FmtToDXGIFmtMap[TEX_FORMAT_R24_UNORM_X8_TYPELESS]  = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        FmtToDXGIFmtMap[TEX_FORMAT_X24_TYPELESS_G8_UINT]   = DXGI_FORMAT_X24_TYPELESS_G8_UINT;

        FmtToDXGIFmtMap[TEX_FORMAT_RG8_TYPELESS]           = DXGI_FORMAT_R8G8_TYPELESS;
        FmtToDXGIFmtMap[TEX_FORMAT_RG8_UNORM]              = DXGI_FORMAT_R8G8_UNORM;
        FmtToDXGIFmtMap[TEX_FORMAT_RG8_UINT]               = DXGI_FORMAT_R8G8_UINT;
        FmtToDXGIFmtMap[TEX_FORMAT_RG8_SNORM]              = DXGI_FORMAT_R8G8_SNORM;
        FmtToDXGIFmtMap[TEX_FORMAT_RG8_SINT]               = DXGI_FORMAT_R8G8_SINT;

        FmtToDXGIFmtMap[TEX_FORMAT_R16_TYPELESS]           = DXGI_FORMAT_R16_TYPELESS;
        FmtToDXGIFmtMap[TEX_FORMAT_R16_FLOAT]              = DXGI_FORMAT_R16_FLOAT;
        FmtToDXGIFmtMap[TEX_FORMAT_D16_UNORM]              = DXGI_FORMAT_D16_UNORM;
        FmtToDXGIFmtMap[TEX_FORMAT_R16_UNORM]              = DXGI_FORMAT_R16_UNORM;
        FmtToDXGIFmtMap[TEX_FORMAT_R16_UINT]               = DXGI_FORMAT_R16_UINT;
        FmtToDXGIFmtMap[TEX_FORMAT_R16_SNORM]              = DXGI_FORMAT_R16_SNORM;
        FmtToDXGIFmtMap[TEX_FORMAT_R16_SINT]               = DXGI_FORMAT_R16_SINT;

        FmtToDXGIFmtMap[TEX_FORMAT_R8_TYPELESS]            = DXGI_FORMAT_R8_TYPELESS;
        FmtToDXGIFmtMap[TEX_FORMAT_R8_UNORM]               = DXGI_FORMAT_R8_UNORM;
        FmtToDXGIFmtMap[TEX_FORMAT_R8_UINT]                = DXGI_FORMAT_R8_UINT;
        FmtToDXGIFmtMap[TEX_FORMAT_R8_SNORM]               = DXGI_FORMAT_R8_SNORM;
        FmtToDXGIFmtMap[TEX_FORMAT_R8_SINT]                = DXGI_FORMAT_R8_SINT;
        FmtToDXGIFmtMap[TEX_FORMAT_A8_UNORM]               = DXGI_FORMAT_A8_UNORM;

        FmtToDXGIFmtMap[TEX_FORMAT_R1_UNORM]               = DXGI_FORMAT_R1_UNORM ;
        FmtToDXGIFmtMap[TEX_FORMAT_RGB9E5_SHAREDEXP]       = DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
        FmtToDXGIFmtMap[TEX_FORMAT_RG8_B8G8_UNORM]         = DXGI_FORMAT_R8G8_B8G8_UNORM;
        FmtToDXGIFmtMap[TEX_FORMAT_G8R8_G8B8_UNORM]        = DXGI_FORMAT_G8R8_G8B8_UNORM;

        FmtToDXGIFmtMap[TEX_FORMAT_BC1_TYPELESS]           = DXGI_FORMAT_BC1_TYPELESS;
        FmtToDXGIFmtMap[TEX_FORMAT_BC1_UNORM]              = DXGI_FORMAT_BC1_UNORM;
        FmtToDXGIFmtMap[TEX_FORMAT_BC1_UNORM_SRGB]         = DXGI_FORMAT_BC1_UNORM_SRGB;
        FmtToDXGIFmtMap[TEX_FORMAT_BC2_TYPELESS]           = DXGI_FORMAT_BC2_TYPELESS;
        FmtToDXGIFmtMap[TEX_FORMAT_BC2_UNORM]              = DXGI_FORMAT_BC2_UNORM;
        FmtToDXGIFmtMap[TEX_FORMAT_BC2_UNORM_SRGB]         = DXGI_FORMAT_BC2_UNORM_SRGB;
        FmtToDXGIFmtMap[TEX_FORMAT_BC3_TYPELESS]           = DXGI_FORMAT_BC3_TYPELESS;
        FmtToDXGIFmtMap[TEX_FORMAT_BC3_UNORM]              = DXGI_FORMAT_BC3_UNORM;
        FmtToDXGIFmtMap[TEX_FORMAT_BC3_UNORM_SRGB]         = DXGI_FORMAT_BC3_UNORM_SRGB;
        FmtToDXGIFmtMap[TEX_FORMAT_BC4_TYPELESS]           = DXGI_FORMAT_BC4_TYPELESS;
        FmtToDXGIFmtMap[TEX_FORMAT_BC4_UNORM]              = DXGI_FORMAT_BC4_UNORM;
        FmtToDXGIFmtMap[TEX_FORMAT_BC4_SNORM]              = DXGI_FORMAT_BC4_SNORM;
        FmtToDXGIFmtMap[TEX_FORMAT_BC5_TYPELESS]           = DXGI_FORMAT_BC5_TYPELESS;
        FmtToDXGIFmtMap[TEX_FORMAT_BC5_UNORM]              = DXGI_FORMAT_BC5_UNORM;
        FmtToDXGIFmtMap[TEX_FORMAT_BC5_SNORM]              = DXGI_FORMAT_BC5_SNORM;

        FmtToDXGIFmtMap[TEX_FORMAT_B5G6R5_UNORM]           = DXGI_FORMAT_B5G6R5_UNORM;
        FmtToDXGIFmtMap[TEX_FORMAT_B5G5R5A1_UNORM]         = DXGI_FORMAT_B5G5R5A1_UNORM;
        FmtToDXGIFmtMap[TEX_FORMAT_BGRA8_UNORM]            = DXGI_FORMAT_B8G8R8A8_UNORM;
        FmtToDXGIFmtMap[TEX_FORMAT_BGRX8_UNORM]            = DXGI_FORMAT_B8G8R8X8_UNORM;

        FmtToDXGIFmtMap[TEX_FORMAT_R10G10B10_XR_BIAS_A2_UNORM]= DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;

        FmtToDXGIFmtMap[TEX_FORMAT_BGRA8_TYPELESS]         = DXGI_FORMAT_B8G8R8A8_TYPELESS;
        FmtToDXGIFmtMap[TEX_FORMAT_BGRA8_UNORM_SRGB]       = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
        FmtToDXGIFmtMap[TEX_FORMAT_BGRX8_TYPELESS]         = DXGI_FORMAT_B8G8R8X8_TYPELESS;
        FmtToDXGIFmtMap[TEX_FORMAT_BGRX8_UNORM_SRGB]       = DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;

        FmtToDXGIFmtMap[TEX_FORMAT_BC6H_TYPELESS]          = DXGI_FORMAT_BC6H_TYPELESS;
        FmtToDXGIFmtMap[TEX_FORMAT_BC6H_UF16]              = DXGI_FORMAT_BC6H_UF16;
        FmtToDXGIFmtMap[TEX_FORMAT_BC6H_SF16]              = DXGI_FORMAT_BC6H_SF16;
        FmtToDXGIFmtMap[TEX_FORMAT_BC7_TYPELESS]           = DXGI_FORMAT_BC7_TYPELESS ;
        FmtToDXGIFmtMap[TEX_FORMAT_BC7_UNORM]              = DXGI_FORMAT_BC7_UNORM;
        FmtToDXGIFmtMap[TEX_FORMAT_BC7_UNORM_SRGB]         = DXGI_FORMAT_BC7_UNORM_SRGB;
        // clang-format on

        static_assert(TEX_FORMAT_NUM_FORMATS == 106, "Please enter the new format information above");
        bFormatMapInitialized = true;
    }

    if (TexFormat >= TEX_FORMAT_UNKNOWN && TexFormat < TEX_FORMAT_NUM_FORMATS)
    {
        DXGI_FORMAT DXGIFormat = FmtToDXGIFmtMap[TexFormat];
        VERIFY(TexFormat == TEX_FORMAT_UNKNOWN || TexFormat > TEX_FORMAT_BC7_UNORM_SRGB || DXGIFormat != DXGI_FORMAT_UNKNOWN, "Unsupported texture format");
        if (BindFlags != 0)
            DXGIFormat = CorrectDXGIFormat(DXGIFormat, BindFlags);
        return DXGIFormat;
    }
    else
    {
        UNEXPECTED("Texture format (", TexFormat, ") is out of allowed range [0, ", TEX_FORMAT_NUM_FORMATS - 1, "]");
        return DXGI_FORMAT_UNKNOWN;
    }
}

class DXGIFmtToFmtMapInitializer
{
public:
    DXGIFmtToFmtMapInitializer(TEXTURE_FORMAT DXGIFmtToFmtMap[])
    {
        for (TEXTURE_FORMAT fmt = TEX_FORMAT_UNKNOWN; fmt < TEX_FORMAT_NUM_FORMATS; fmt = static_cast<TEXTURE_FORMAT>(fmt + 1))
        {
            DXGI_FORMAT DXGIFmt = TexFormatToDXGI_Format(fmt);
            VERIFY_EXPR(DXGIFmt <= DXGI_FORMAT_B4G4R4A4_UNORM);
            DXGIFmtToFmtMap[DXGIFmt] = fmt;
        }
    }
};

TEXTURE_FORMAT DXGI_FormatToTexFormat(DXGI_FORMAT DXGIFormat)
{
    static_assert(DXGI_FORMAT_B4G4R4A4_UNORM == 115, "Unexpected DXGI format value");
    static TEXTURE_FORMAT             DXGIFmtToFmtMap[DXGI_FORMAT_B4G4R4A4_UNORM + 1];
    static DXGIFmtToFmtMapInitializer Initializer(DXGIFmtToFmtMap);

    if (DXGIFormat >= DXGI_FORMAT_UNKNOWN && DXGIFormat <= DXGI_FORMAT_BC7_UNORM_SRGB)
    {
        TEXTURE_FORMAT Format = DXGIFmtToFmtMap[DXGIFormat];
        VERIFY(DXGIFormat == DXGI_FORMAT_UNKNOWN || Format != TEX_FORMAT_UNKNOWN, "Unsupported texture format");
        VERIFY_EXPR(DXGIFormat == TexFormatToDXGI_Format(Format));
        return Format;
    }
    else
    {
        UNEXPECTED("DXGI texture format (", DXGIFormat, ") is out of allowed range [0, ", DXGI_FORMAT_BC7_UNORM_SRGB, "]");
        return TEX_FORMAT_UNKNOWN;
    }
}

} // namespace Diligent
