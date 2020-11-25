#pragma once

#include <QImageIOHandler>
#include <QSize>
#include <QRect>
#include <QSharedPointer>

#include "PVRCore/Texture/TextureHeader.h"

namespace pvr
{
class Texture;
}
class PVRTextureWrapper;

class QPVRHandler : public QImageIOHandler
{
public:
	QPVRHandler();

	enum Format
	{
		UnknownFormat = 0,
		PVR = (1 << 0),
		PVR2 = (1 << 2) | PVR,
		PVR3 = (1 << 3) | PVR,
		ETC = (1 << 4),
		PVRTC = (1 << 5),
		TEXV2 = (1 << 6),
		TEXBIT4 = (1 << 7),
		NOALPHA = (1 << 8),
		PREMULTIPLIED = (1 << 9),
		PVR2_PVRTC1_2 = PVR2 | PVRTC,
		PVR3_PVRTC1_2 = PVR3 | PVRTC,
		PVR2_PVRTC1_2_NOALPHA = PVR2 | PVRTC | NOALPHA,
		PVR3_PVRTC1_2_NOALPHA = PVR3 | PVRTC | NOALPHA,
		PVR2_PVRTC1_2_PREMULTIPLIED = PVR2 | PVRTC | PREMULTIPLIED,
		PVR3_PVRTC1_2_PREMULTIPLIED = PVR3 | PVRTC | PREMULTIPLIED,
		PVR2_PVRTC1_4 = PVR2 | TEXBIT4 | PVRTC,
		PVR3_PVRTC1_4 = PVR3 | TEXBIT4 | PVRTC,
		PVR2_PVRTC1_4_NOALPHA = PVR2 | TEXBIT4 | PVRTC | NOALPHA,
		PVR3_PVRTC1_4_NOALPHA = PVR3 | TEXBIT4 | PVRTC | NOALPHA,
		PVR2_PVRTC1_4_PREMULTIPLIED = PVR2 | TEXBIT4 | PVRTC | PREMULTIPLIED,
		PVR3_PVRTC1_4_PREMULTIPLIED = PVR3 | TEXBIT4 | PVRTC | PREMULTIPLIED,
		PVR2_PVRTC2_2 = PVR2 | PVRTC | TEXV2,
		PVR3_PVRTC2_2 = PVR3 | PVRTC | TEXV2,
		PVR2_PVRTC2_4 = PVR2 | TEXBIT4 | PVRTC | TEXV2,
		PVR3_PVRTC2_4 = PVR3 | TEXBIT4 | PVRTC | TEXV2,
		PVR2_PVRTC2_2_PREMULTIPLIED = PVR2 | PVRTC | TEXV2 | PREMULTIPLIED,
		PVR3_PVRTC2_2_PREMULTIPLIED = PVR3 | PVRTC | TEXV2 | PREMULTIPLIED,
		PVR2_PVRTC2_4_PREMULTIPLIED =
			PVR2 | TEXBIT4 | PVRTC | TEXV2 | PREMULTIPLIED,
		PVR3_PVRTC2_4_PREMULTIPLIED =
			PVR3 | TEXBIT4 | PVRTC | TEXV2 | PREMULTIPLIED,
		PVR2_ETC1 = PVR2 | ETC,
		PVR3_ETC1 = PVR3 | ETC,
		PVR3_ETC2 = PVR3 | ETC | TEXV2,
		PVR3_ETC2_NOALPHA = PVR3 | ETC | TEXV2 | NOALPHA,
		PVR3_ETC2_PREMULTIPLIED = PVR3 | ETC | TEXV2 | PREMULTIPLIED,
		FileFormatMask = PVR
	};

	static Format detectFileFormat(QIODevice *device);

	virtual bool canRead() const override;
	virtual bool read(QImage *image) override;
	virtual bool write(const QImage &image) override;

	virtual QVariant option(ImageOption option) const override;
	virtual void setOption(ImageOption option, const QVariant &value) override;
	virtual bool supportsOption(ImageOption option) const override;

	virtual int imageCount() const override;
	virtual int currentImageNumber() const override;
	virtual bool jumpToImage(int imageNumber) override;
	virtual bool jumpToNextImage() override;

	static QByteArray PVR_Format();

private:
	static bool pvrPixelTypeHasAlpha(quint64 pixelTypeId);

	static bool stringToFormat(
		const QByteArray &str, int *format, int *imageFormat);
	static QByteArray formatToString(int format);
	static QByteArray subType(int format, int imageFormat);
	static const QList<QByteArray> &supportedSubTypes();

	bool ensureScanned() const;
	bool scanDevice() const;
	bool writeTexture(const PVRTextureWrapper &texture);

	quint32 getCurrentMipLevel() const;
	quint32 getCurrentArrayIndex() const;
	quint32 getCurrentFaceIndex() const;

	static void QImageTextureCleanup(void *ptr);

	using TexturePtr = QSharedPointer<PVRTextureWrapper>;
	mutable TexturePtr mTexture;
	QSize mScaledSize;
	int mQuality;
	mutable int mOrientation;
	mutable int mFormat;
	mutable int mImageFormat;
	mutable int mImageCurrentIndex;
	mutable bool mScanned;
};
