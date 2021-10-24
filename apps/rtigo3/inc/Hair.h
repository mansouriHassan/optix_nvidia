//
// Copyright (c) 2020, NVIDIA CORPORATION. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
#pragma once

#include <optix.h>
//#include <optix_stubs.h>

#include<SceneGraph.h>

#include "shaders/vertex_attributes.h"
#include "shaders/vector_math.h"

#include <ostream>
#include <iostream>
#include <string>
#include <vector>

namespace sg
{
    class Curves : public Node
    {
    public:
        enum SplineMode
        {
            LINEAR_BSPLINE,
            QUADRATIC_BSPLINE,
            CUBIC_BSPLINE
        };
        enum Shade
        {
            SEGMENT_U,
            STRAND_U,
            STRAND_IDX
        };
        enum Radius
        {
            CONSTANT_R,
            TAPERED_R
        };

        Curves(const unsigned int id);
        Curves(const unsigned int id, float density, float disparity);
        void createHairFromFile(const std::string& fileName);
        void createHairFromFile(const std::string& fileName, const bool side);

        sg::NodeType getType() const;

        void setAttributes(std::vector<VertexAttributes> const& attributes);
        std::vector<VertexAttributes> const& getAttributes() const;

        void setIndices(std::vector<unsigned int> const&);
        std::vector<unsigned int> const& getIndices() const;

        void setThickness(std::vector<float> const&);
        std::vector<float> const& getThickness() const;

        virtual ~Curves();

        // Factory method for loading Hair from file.
        // static Hair Load( const std::string& fileName, const OptixDeviceContext context );

        void       setSplineMode(SplineMode splineMode) { m_splineMode = splineMode; };
        SplineMode splineMode() const { return m_splineMode; };

        //void  setShadeMode( Shade shadeMode ) { m_shadeMode = shadeMode; };
        //Shade shadeMode() const { return m_shadeMode; };

        void   setRadiusMode(Radius radiusMode);
        Radius radiusMode() const { return m_radiusMode; };

        uint32_t    numberOfStrands() const;
        uint32_t    numberOfPoints() const;
        std::string fileInfo() const;

        std::vector<float3> points() const;
        std::vector<float>  widths() const;

        int numberOfSegments() const;

        // Compute a vector containing vertex indices for all segments
        // making up the hair geometry. E.g.
        // [h0s0, h0s1, ..., h0sn0, h1s0, h1s1, ..., h1sn1, h2s0, ...]
        //
        std::vector<unsigned int> segments() const;

        std::vector<float2> strandU() const;
        std::vector<int>    strandIndices() const;
        std::vector<float3> strandRand() const;
        std::vector<uint2>  strandInfo() const;

        //virtual void gatherProgramGroups( HairProgramGroups* pProgramGroups ) const;

        std::string programName() const;
        std::string programSuffix() const;

        unsigned int usesPrimitiveTypes() const
        {
            switch (m_splineMode) {
            case LINEAR_BSPLINE:
                return OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_LINEAR;
            case QUADRATIC_BSPLINE:
                return OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_QUADRATIC_BSPLINE;
            case CUBIC_BSPLINE:
                return OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_CUBIC_BSPLINE;
            default:
                std::cout << "Invalid b-spline mode." << std::endl;
            }
        }

    protected:
        bool hasSegments() const;
        bool hasPoints() const;
        bool hasThickness() const;
        bool hasAlpha() const;
        bool hasColor() const;

        //OptixTraversableHandle gas() const;

        uint32_t defaultNumberOfSegments() const;
        float    defaultThickness() const;
        float    defaultAlpha() const;
        float3   defaultColor() const;

        //void makeOptix() const;
        //void clearOptix();


    private:

        // .hair format spec here: http://www.cemyuksel.com/research/hairmodels/
        struct FileHeader
        {
            // Bytes 0 - 3  Must be "HAIR" in ascii code(48 41 49 52)
            char magic[4];

            // Bytes 4 - 7  Number of hair strands as unsigned int
            uint32_t numStrands;

            // Bytes 8 - 11  Total number of points of all strands as unsigned int
            uint32_t numPoints;

            // Bytes 12 - 15  Bit array of data in the file
            // Bit - 5 to Bit - 31 are reserved for future extension(must be 0).
            uint32_t flags;

            // Bytes 16 - 19  Default number of segments of hair strands as unsigned int
            // If the file does not have a segments array, this default value is used.
            uint32_t defaultNumSegments;

            // Bytes 20 - 23  Default thickness hair strands as float
            // If the file does not have a thickness array, this default value is used.
            float defaultThickness;

            // Bytes 24 - 27  Default transparency hair strands as float
            // If the file does not have a transparency array, this default value is used.
            float defaultAlpha;

            // Bytes 28 - 39  Default color hair strands as float array of size 3
            // If the file does not have a color array, this default value is used.
            float3 defaultColor;

            // Bytes 40 - 127  File information as char array of size 88 in ascii
            char fileInfo[88];
        };

        FileHeader          m_header;
        std::vector<int>    m_strands;
        std::vector<float3> m_points;
        std::vector<float>  m_thickness;

        float m_density;
        float m_disparity;

        std::vector<VertexAttributes> m_attributes;
        std::vector<unsigned int>       m_indices;

        SplineMode   m_splineMode = QUADRATIC_BSPLINE;
        unsigned int curveDegree() const
        {
            switch (m_splineMode) {
            case LINEAR_BSPLINE:
                return 1;
            case QUADRATIC_BSPLINE:
                return 2;
            case CUBIC_BSPLINE:
                return 3;
            default:
                std::cout << "Invalid spline mode." << std::endl;
                return 0;
            }
        }
        Shade  m_shadeMode = SEGMENT_U;
        Radius m_radiusMode = CONSTANT_R;

        //OptixDeviceContext m_context = 0;

        friend std::ostream& operator<<(std::ostream& o, const Curves& curves);
    };

    // Ouput operator for Hair
    std::ostream& operator<<(std::ostream& o, const Curves& curves);
}