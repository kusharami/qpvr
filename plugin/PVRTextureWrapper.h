#pragma once

#include "PVRCore/Texture.h"

enum ECompressorQuality
{
	ePVRTCFastest = 0, //!< PVRTC fastest
	ePVRTCFast, //!< PVRTC fast
	ePVRTCNormal, //!< PVRTC normal
	ePVRTCHigh, //!< PVRTC high
	ePVRTCBest, //!< PVRTC best
	eNumPVRTCModes, //!< Number of PVRTC modes

	eETCFast = 0, //!< ETC fast
	eETCFastPerceptual, //!< ETC fast perceptual
	eETCSlow, //!< ETC slow
	eETCSlowPerceptual, //!< ETC slow perceptual
	eNumETCModes //!< Number of ETC modes
};

class PVRTextureWrapper : public pvr::Texture
{
public:
	PVRTextureWrapper() = default;
	PVRTextureWrapper(const pvr::PixelFormat &ptFormat, uint32_t width,
		uint32_t height, int orientation, bool premultiplied);
	PVRTextureWrapper(const pvr::Texture &other);
	PVRTextureWrapper(
		const pvr::TextureHeader &sHeader, const void *pData = nullptr);

	bool transcode(const pvr::PixelFormat &ptFormat, int eQuality);

private:
	void setTextureData(const pvr::PixelFormat &ptFormat, const void *data);
};
