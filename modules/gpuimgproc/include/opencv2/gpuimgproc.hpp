/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#ifndef __OPENCV_GPUIMGPROC_HPP__
#define __OPENCV_GPUIMGPROC_HPP__

#ifndef __cplusplus
#  error gpuimgproc.hpp header must be compiled as C++
#endif

#include "opencv2/core/gpu.hpp"
#include "opencv2/imgproc.hpp"

#if defined __GNUC__
    #define __OPENCV_GPUIMGPROC_DEPR_BEFORE__
    #define __OPENCV_GPUIMGPROC_DEPR_AFTER__ __attribute__ ((deprecated))
#elif (defined WIN32 || defined _WIN32)
    #define __OPENCV_GPUIMGPROC_DEPR_BEFORE__ __declspec(deprecated)
    #define __OPENCV_GPUIMGPROC_DEPR_AFTER__
#else
    #define __OPENCV_GPUIMGPROC_DEPR_BEFORE__
    #define __OPENCV_GPUIMGPROC_DEPR_AFTER__
#endif

namespace cv { namespace gpu {

/////////////////////////// Color Processing ///////////////////////////

//! converts image from one color space to another
CV_EXPORTS void cvtColor(InputArray src, OutputArray dst, int code, int dcn = 0, Stream& stream = Stream::Null());

enum
{
    // Bayer Demosaicing (Malvar, He, and Cutler)
    COLOR_BayerBG2BGR_MHT = 256,
    COLOR_BayerGB2BGR_MHT = 257,
    COLOR_BayerRG2BGR_MHT = 258,
    COLOR_BayerGR2BGR_MHT = 259,

    COLOR_BayerBG2RGB_MHT = COLOR_BayerRG2BGR_MHT,
    COLOR_BayerGB2RGB_MHT = COLOR_BayerGR2BGR_MHT,
    COLOR_BayerRG2RGB_MHT = COLOR_BayerBG2BGR_MHT,
    COLOR_BayerGR2RGB_MHT = COLOR_BayerGB2BGR_MHT,

    COLOR_BayerBG2GRAY_MHT = 260,
    COLOR_BayerGB2GRAY_MHT = 261,
    COLOR_BayerRG2GRAY_MHT = 262,
    COLOR_BayerGR2GRAY_MHT = 263
};
CV_EXPORTS void demosaicing(InputArray src, OutputArray dst, int code, int dcn = -1, Stream& stream = Stream::Null());

//! swap channels
//! dstOrder - Integer array describing how channel values are permutated. The n-th entry
//!            of the array contains the number of the channel that is stored in the n-th channel of
//!            the output image. E.g. Given an RGBA image, aDstOrder = [3,2,1,0] converts this to ABGR
//!            channel order.
CV_EXPORTS void swapChannels(InputOutputArray image, const int dstOrder[4], Stream& stream = Stream::Null());

//! Routines for correcting image color gamma
CV_EXPORTS void gammaCorrection(InputArray src, OutputArray dst, bool forward = true, Stream& stream = Stream::Null());

enum { ALPHA_OVER, ALPHA_IN, ALPHA_OUT, ALPHA_ATOP, ALPHA_XOR, ALPHA_PLUS, ALPHA_OVER_PREMUL, ALPHA_IN_PREMUL, ALPHA_OUT_PREMUL,
       ALPHA_ATOP_PREMUL, ALPHA_XOR_PREMUL, ALPHA_PLUS_PREMUL, ALPHA_PREMUL};

//! Composite two images using alpha opacity values contained in each image
//! Supports CV_8UC4, CV_16UC4, CV_32SC4 and CV_32FC4 types
CV_EXPORTS void alphaComp(InputArray img1, InputArray img2, OutputArray dst, int alpha_op, Stream& stream = Stream::Null());

////////////////////////////// Histogram ///////////////////////////////

//! Calculates histogram for 8u one channel image
//! Output hist will have one row, 256 cols and CV32SC1 type.
CV_EXPORTS void calcHist(InputArray src, OutputArray hist, Stream& stream = Stream::Null());

//! normalizes the grayscale image brightness and contrast by normalizing its histogram
CV_EXPORTS void equalizeHist(InputArray src, OutputArray dst, InputOutputArray buf, Stream& stream = Stream::Null());

static inline void equalizeHist(InputArray src, OutputArray dst, Stream& stream = Stream::Null())
{
    GpuMat buf;
    gpu::equalizeHist(src, dst, buf, stream);
}

class CV_EXPORTS CLAHE : public cv::CLAHE
{
public:
    using cv::CLAHE::apply;
    virtual void apply(InputArray src, OutputArray dst, Stream& stream) = 0;
};
CV_EXPORTS Ptr<gpu::CLAHE> createCLAHE(double clipLimit = 40.0, Size tileGridSize = Size(8, 8));

//! Compute levels with even distribution. levels will have 1 row and nLevels cols and CV_32SC1 type.
CV_EXPORTS void evenLevels(OutputArray levels, int nLevels, int lowerLevel, int upperLevel);

//! Calculates histogram with evenly distributed bins for signle channel source.
//! Supports CV_8UC1, CV_16UC1 and CV_16SC1 source types.
//! Output hist will have one row and histSize cols and CV_32SC1 type.
CV_EXPORTS void histEven(InputArray src, OutputArray hist, InputOutputArray buf, int histSize, int lowerLevel, int upperLevel, Stream& stream = Stream::Null());

static inline void histEven(InputArray src, OutputArray hist, int histSize, int lowerLevel, int upperLevel, Stream& stream = Stream::Null())
{
    GpuMat buf;
    gpu::histEven(src, hist, buf, histSize, lowerLevel, upperLevel, stream);
}

//! Calculates histogram with evenly distributed bins for four-channel source.
//! All channels of source are processed separately.
//! Supports CV_8UC4, CV_16UC4 and CV_16SC4 source types.
//! Output hist[i] will have one row and histSize[i] cols and CV_32SC1 type.
CV_EXPORTS void histEven(InputArray src, GpuMat hist[4], InputOutputArray buf, int histSize[4], int lowerLevel[4], int upperLevel[4], Stream& stream = Stream::Null());

static inline void histEven(InputArray src, GpuMat hist[4], int histSize[4], int lowerLevel[4], int upperLevel[4], Stream& stream = Stream::Null())
{
    GpuMat buf;
    gpu::histEven(src, hist, buf, histSize, lowerLevel, upperLevel, stream);
}

//! Calculates histogram with bins determined by levels array.
//! levels must have one row and CV_32SC1 type if source has integer type or CV_32FC1 otherwise.
//! Supports CV_8UC1, CV_16UC1, CV_16SC1 and CV_32FC1 source types.
//! Output hist will have one row and (levels.cols-1) cols and CV_32SC1 type.
CV_EXPORTS void histRange(InputArray src, OutputArray hist, InputArray levels, InputOutputArray buf, Stream& stream = Stream::Null());

static inline void histRange(InputArray src, OutputArray hist, InputArray levels, Stream& stream = Stream::Null())
{
    GpuMat buf;
    gpu::histRange(src, hist, levels, buf, stream);
}

//! Calculates histogram with bins determined by levels array.
//! All levels must have one row and CV_32SC1 type if source has integer type or CV_32FC1 otherwise.
//! All channels of source are processed separately.
//! Supports CV_8UC4, CV_16UC4, CV_16SC4 and CV_32FC4 source types.
//! Output hist[i] will have one row and (levels[i].cols-1) cols and CV_32SC1 type.
CV_EXPORTS void histRange(InputArray src, GpuMat hist[4], const GpuMat levels[4], InputOutputArray buf, Stream& stream = Stream::Null());

static inline void histRange(InputArray src, GpuMat hist[4], const GpuMat levels[4], Stream& stream = Stream::Null())
{
    GpuMat buf;
    gpu::histRange(src, hist, levels, buf, stream);
}

//////////////////////////////// Canny ////////////////////////////////

class CV_EXPORTS CannyEdgeDetector : public Algorithm
{
public:
    virtual void detect(InputArray image, OutputArray edges) = 0;
    virtual void detect(InputArray dx, InputArray dy, OutputArray edges) = 0;

    virtual void setLowThreshold(double low_thresh) = 0;
    virtual double getLowThreshold() const = 0;

    virtual void setHighThreshold(double high_thresh) = 0;
    virtual double getHighThreshold() const = 0;

    virtual void setAppertureSize(int apperture_size) = 0;
    virtual int getAppertureSize() const = 0;

    virtual void setL2Gradient(bool L2gradient) = 0;
    virtual bool getL2Gradient() const = 0;
};

CV_EXPORTS Ptr<CannyEdgeDetector> createCannyEdgeDetector(double low_thresh, double high_thresh, int apperture_size = 3, bool L2gradient = false);

// obsolete

__OPENCV_GPUIMGPROC_DEPR_BEFORE__ void Canny(InputArray image, OutputArray edges,
                                             double low_thresh, double high_thresh, int apperture_size = 3, bool L2gradient = false) __OPENCV_GPUIMGPROC_DEPR_AFTER__;
inline void Canny(InputArray image, OutputArray edges, double low_thresh, double high_thresh, int apperture_size, bool L2gradient)
{
    gpu::createCannyEdgeDetector(low_thresh, high_thresh, apperture_size, L2gradient)->detect(image, edges);
}

__OPENCV_GPUIMGPROC_DEPR_BEFORE__ void Canny(InputArray dx, InputArray dy, OutputArray edges,
                                             double low_thresh, double high_thresh, bool L2gradient = false) __OPENCV_GPUIMGPROC_DEPR_AFTER__;
inline void Canny(InputArray dx, InputArray dy, OutputArray edges, double low_thresh, double high_thresh, bool L2gradient)
{
    gpu::createCannyEdgeDetector(low_thresh, high_thresh, 3, L2gradient)->detect(dx, dy, edges);
}

/////////////////////////// Hough Transform ////////////////////////////

//////////////////////////////////////
// HoughLines

class CV_EXPORTS HoughLinesDetector : public Algorithm
{
public:
    virtual void detect(InputArray src, OutputArray lines) = 0;
    virtual void downloadResults(InputArray d_lines, OutputArray h_lines, OutputArray h_votes = noArray()) = 0;

    virtual void setRho(float rho) = 0;
    virtual float getRho() const = 0;

    virtual void setTheta(float theta) = 0;
    virtual float getTheta() const = 0;

    virtual void setThreshold(int threshold) = 0;
    virtual int getThreshold() const = 0;

    virtual void setDoSort(bool doSort) = 0;
    virtual bool getDoSort() const = 0;

    virtual void setMaxLines(int maxLines) = 0;
    virtual int getMaxLines() const = 0;
};

CV_EXPORTS Ptr<HoughLinesDetector> createHoughLinesDetector(float rho, float theta, int threshold, bool doSort = false, int maxLines = 4096);

// obsolete

__OPENCV_GPUIMGPROC_DEPR_BEFORE__ void HoughLines(InputArray src, OutputArray lines, float rho, float theta, int threshold,
                                                  bool doSort = false, int maxLines = 4096) __OPENCV_GPUIMGPROC_DEPR_AFTER__;

inline void HoughLines(InputArray src, OutputArray lines, float rho, float theta, int threshold, bool doSort, int maxLines)
{
    gpu::createHoughLinesDetector(rho, theta, threshold, doSort, maxLines)->detect(src, lines);
}

//////////////////////////////////////
// HoughLinesP

//! finds line segments in the black-n-white image using probabalistic Hough transform
class CV_EXPORTS HoughSegmentDetector : public Algorithm
{
public:
    virtual void detect(InputArray src, OutputArray lines) = 0;

    virtual void setRho(float rho) = 0;
    virtual float getRho() const = 0;

    virtual void setTheta(float theta) = 0;
    virtual float getTheta() const = 0;

    virtual void setMinLineLength(int minLineLength) = 0;
    virtual int getMinLineLength() const = 0;

    virtual void setMaxLineGap(int maxLineGap) = 0;
    virtual int getMaxLineGap() const = 0;

    virtual void setMaxLines(int maxLines) = 0;
    virtual int getMaxLines() const = 0;
};

CV_EXPORTS Ptr<HoughSegmentDetector> createHoughSegmentDetector(float rho, float theta, int minLineLength, int maxLineGap, int maxLines = 4096);

// obsolete

__OPENCV_GPUIMGPROC_DEPR_BEFORE__ void HoughLinesP(InputArray src, OutputArray lines,
                                                   float rho, float theta, int minLineLength, int maxLineGap, int maxLines = 4096) __OPENCV_GPUIMGPROC_DEPR_AFTER__;

inline void HoughLinesP(InputArray src, OutputArray lines, float rho, float theta, int minLineLength, int maxLineGap, int maxLines)
{
    gpu::createHoughSegmentDetector(rho, theta, minLineLength, maxLineGap, maxLines)->detect(src, lines);
}

//////////////////////////////////////
// HoughCircles

class CV_EXPORTS HoughCirclesDetector : public Algorithm
{
public:
    virtual void detect(InputArray src, OutputArray circles) = 0;

    virtual void setDp(float dp) = 0;
    virtual float getDp() const = 0;

    virtual void setMinDist(float minDist) = 0;
    virtual float getMinDist() const = 0;

    virtual void setCannyThreshold(int cannyThreshold) = 0;
    virtual int getCannyThreshold() const = 0;

    virtual void setVotesThreshold(int votesThreshold) = 0;
    virtual int getVotesThreshold() const = 0;

    virtual void setMinRadius(int minRadius) = 0;
    virtual int getMinRadius() const = 0;

    virtual void setMaxRadius(int maxRadius) = 0;
    virtual int getMaxRadius() const = 0;

    virtual void setMaxCircles(int maxCircles) = 0;
    virtual int getMaxCircles() const = 0;
};

CV_EXPORTS Ptr<HoughCirclesDetector> createHoughCirclesDetector(float dp, float minDist, int cannyThreshold, int votesThreshold, int minRadius, int maxRadius, int maxCircles = 4096);

// obsolete

__OPENCV_GPUIMGPROC_DEPR_BEFORE__ void HoughCircles(InputArray src, OutputArray circles,
                                                    int method, float dp, float minDist, int cannyThreshold, int votesThreshold,
                                                    int minRadius, int maxRadius, int maxCircles = 4096) __OPENCV_GPUIMGPROC_DEPR_AFTER__;

inline void HoughCircles(InputArray src, OutputArray circles, int /*method*/, float dp, float minDist,
                         int cannyThreshold, int votesThreshold, int minRadius, int maxRadius, int maxCircles)
{
    gpu::createHoughCirclesDetector(dp, minDist, cannyThreshold, votesThreshold, minRadius, maxRadius, maxCircles)->detect(src, circles);
}

//////////////////////////////////////
// GeneralizedHough

//! finds arbitrary template in the grayscale image using Generalized Hough Transform
//! Ballard, D.H. (1981). Generalizing the Hough transform to detect arbitrary shapes. Pattern Recognition 13 (2): 111-122.
//! Guil, N., González-Linares, J.M. and Zapata, E.L. (1999). Bidimensional shape detection using an invariant approach. Pattern Recognition 32 (6): 1025-1038.
class CV_EXPORTS GeneralizedHough : public Algorithm
{
public:
    static Ptr<GeneralizedHough> create(int method);

    //! set template to search
    virtual void setTemplate(InputArray templ, int cannyThreshold = 100, Point templCenter = Point(-1, -1)) = 0;
    virtual void setTemplate(InputArray edges, InputArray dx, InputArray dy, Point templCenter = Point(-1, -1)) = 0;

    //! find template on image
    virtual void detect(InputArray image, OutputArray positions, int cannyThreshold = 100) = 0;
    virtual void detect(InputArray edges, InputArray dx, InputArray dy, OutputArray positions) = 0;

    virtual void downloadResults(InputArray d_positions, OutputArray h_positions, OutputArray h_votes = noArray()) = 0;
};

////////////////////////// Corners Detection ///////////////////////////

class CV_EXPORTS CornernessCriteria : public Algorithm
{
public:
    virtual void compute(InputArray src, OutputArray dst, Stream& stream = Stream::Null()) = 0;
};

//! computes Harris cornerness criteria at each image pixel
CV_EXPORTS Ptr<CornernessCriteria> createHarrisCorner(int srcType, int blockSize, int ksize, double k, int borderType = BORDER_REFLECT101);

//! computes minimum eigen value of 2x2 derivative covariation matrix at each pixel - the cornerness criteria
CV_EXPORTS Ptr<CornernessCriteria> createMinEigenValCorner(int srcType, int blockSize, int ksize, int borderType = BORDER_REFLECT101);

// obsolete

__OPENCV_GPUIMGPROC_DEPR_BEFORE__ void cornerHarris(InputArray src, OutputArray dst,
                                                    int blockSize, int ksize, double k, int borderType = BORDER_REFLECT101,
                                                    Stream& stream = Stream::Null()) __OPENCV_GPUIMGPROC_DEPR_AFTER__;

inline void cornerHarris(InputArray src, OutputArray dst, int blockSize, int ksize, double k, int borderType, Stream& stream)
{
    gpu::createHarrisCorner(src.type(), blockSize, ksize, k, borderType)->compute(src, dst, stream);
}

__OPENCV_GPUIMGPROC_DEPR_BEFORE__ void cornerMinEigenVal(InputArray src, OutputArray dst,
                                                         int blockSize, int ksize, int borderType = BORDER_REFLECT101,
                                                         Stream& stream = Stream::Null()) __OPENCV_GPUIMGPROC_DEPR_AFTER__;

inline void cornerMinEigenVal(InputArray src, OutputArray dst, int blockSize, int ksize, int borderType, Stream& stream)
{
    gpu::createMinEigenValCorner(src.type(), blockSize, ksize, borderType)->compute(src, dst, stream);
}

////////////////////////// Feature Detection ///////////////////////////

class CV_EXPORTS GoodFeaturesToTrackDetector_GPU
{
public:
    explicit GoodFeaturesToTrackDetector_GPU(int maxCorners = 1000, double qualityLevel = 0.01, double minDistance = 0.0,
        int blockSize = 3, bool useHarrisDetector = false, double harrisK = 0.04);

    //! return 1 rows matrix with CV_32FC2 type
    void operator ()(const GpuMat& image, GpuMat& corners, const GpuMat& mask = GpuMat());

    int maxCorners;
    double qualityLevel;
    double minDistance;

    int blockSize;
    bool useHarrisDetector;
    double harrisK;

    void releaseMemory()
    {
        Dx_.release();
        Dy_.release();
        buf_.release();
        eig_.release();
        minMaxbuf_.release();
        tmpCorners_.release();
    }

private:
    GpuMat Dx_;
    GpuMat Dy_;
    GpuMat buf_;
    GpuMat eig_;
    GpuMat minMaxbuf_;
    GpuMat tmpCorners_;
};

inline GoodFeaturesToTrackDetector_GPU::GoodFeaturesToTrackDetector_GPU(int maxCorners_, double qualityLevel_, double minDistance_,
        int blockSize_, bool useHarrisDetector_, double harrisK_)
{
    maxCorners = maxCorners_;
    qualityLevel = qualityLevel_;
    minDistance = minDistance_;
    blockSize = blockSize_;
    useHarrisDetector = useHarrisDetector_;
    harrisK = harrisK_;
}

///////////////////////////// Mean Shift //////////////////////////////

//! Does mean shift filtering on GPU.
CV_EXPORTS void meanShiftFiltering(const GpuMat& src, GpuMat& dst, int sp, int sr,
                                   TermCriteria criteria = TermCriteria(TermCriteria::MAX_ITER + TermCriteria::EPS, 5, 1),
                                   Stream& stream = Stream::Null());

//! Does mean shift procedure on GPU.
CV_EXPORTS void meanShiftProc(const GpuMat& src, GpuMat& dstr, GpuMat& dstsp, int sp, int sr,
                              TermCriteria criteria = TermCriteria(TermCriteria::MAX_ITER + TermCriteria::EPS, 5, 1),
                              Stream& stream = Stream::Null());

//! Does mean shift segmentation with elimination of small regions.
CV_EXPORTS void meanShiftSegmentation(const GpuMat& src, Mat& dst, int sp, int sr, int minsize,
                                      TermCriteria criteria = TermCriteria(TermCriteria::MAX_ITER + TermCriteria::EPS, 5, 1));

/////////////////////////// Match Template ////////////////////////////

struct CV_EXPORTS MatchTemplateBuf
{
    Size user_block_size;
    GpuMat imagef, templf;
    std::vector<GpuMat> images;
    std::vector<GpuMat> image_sums;
    std::vector<GpuMat> image_sqsums;
};

//! computes the proximity map for the raster template and the image where the template is searched for
CV_EXPORTS void matchTemplate(const GpuMat& image, const GpuMat& templ, GpuMat& result, int method, Stream &stream = Stream::Null());

//! computes the proximity map for the raster template and the image where the template is searched for
CV_EXPORTS void matchTemplate(const GpuMat& image, const GpuMat& templ, GpuMat& result, int method, MatchTemplateBuf &buf, Stream& stream = Stream::Null());

////////////////////////// Bilateral Filter ///////////////////////////

//! Performa bilateral filtering of passsed image
CV_EXPORTS void bilateralFilter(const GpuMat& src, GpuMat& dst, int kernel_size, float sigma_color, float sigma_spatial,
                                int borderMode = BORDER_DEFAULT, Stream& stream = Stream::Null());

///////////////////////////// Blending ////////////////////////////////

//! performs linear blending of two images
//! to avoid accuracy errors sum of weigths shouldn't be very close to zero
CV_EXPORTS void blendLinear(const GpuMat& img1, const GpuMat& img2, const GpuMat& weights1, const GpuMat& weights2,
                            GpuMat& result, Stream& stream = Stream::Null());

}} // namespace cv { namespace gpu {

#undef __OPENCV_GPUIMGPROC_DEPR_BEFORE__
#undef __OPENCV_GPUIMGPROC_DEPR_AFTER__

#endif /* __OPENCV_GPUIMGPROC_HPP__ */
