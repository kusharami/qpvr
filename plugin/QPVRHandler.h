#pragma once

#include <QImageIOHandler>
#include <QSize>
#include <QSharedPointer>

#include "PVRCore/Texture/TextureHeader.h"
#include "PVRTexture.h"

namespace pvr
{
class Texture;
}

class QPVRHandler : public QImageIOHandler
{
public:
	QPVRHandler();

	enum Format
	{
		UnknownFormat = 0,
		PVR = (1 << 0),
		CCZ = (1 << 1),
		PVR2 = (1 << 2) | PVR,
		PVR3 = (1 << 3) | PVR,
		ETC = (1 << 4),
		PVRTC = (1 << 5),
		TEXV2 = (1 << 6),
		TEXBIT4 = (1 << 7),
		PVR2_PVRTC1_2 = PVR2 | PVRTC,
		PVR3_PVRTC1_2 = PVR3 | PVRTC,
		PVR2_PVRTC1_4 = PVR2 | TEXBIT4 | PVRTC,
		PVR3_PVRTC1_4 = PVR3 | TEXBIT4 | PVRTC,
		PVR2_PVRTC2_2 = PVR2 | PVRTC | TEXV2,
		PVR3_PVRTC2_2 = PVR3 | PVRTC | TEXV2,
		PVR2_PVRTC2_4 = PVR2 | TEXBIT4 | PVRTC | TEXV2,
		PVR3_PVRTC2_4 = PVR3 | TEXBIT4 | PVRTC | TEXV2,
		PVR2_ETC1 = PVR2 | ETC,
		PVR3_ETC1 = PVR3 | ETC,
		PVR3_ETC2 = PVR3 | ETC | TEXV2,
		PVR2_CCZ = PVR2 | CCZ,
		PVR3_CCZ = PVR3 | CCZ,
		PVR2_PVRTC1_2_CCZ = PVR2_PVRTC1_2 | CCZ,
		PVR3_PVRTC1_2_CCZ = PVR3_PVRTC1_2 | CCZ,
		PVR2_PVRTC1_4_CCZ = PVR2_PVRTC1_4 | CCZ,
		PVR3_PVRTC1_4_CCZ = PVR3_PVRTC1_4 | CCZ,
		PVR2_PVRTC2_2_CCZ = PVR2_PVRTC2_2 | CCZ,
		PVR3_PVRTC2_2_CCZ = PVR3_PVRTC2_2 | CCZ,
		PVR2_PVRTC2_4_CCZ = PVR2_PVRTC2_4 | CCZ,
		PVR3_PVRTC2_4_CCZ = PVR3_PVRTC2_4 | CCZ,
		PVR2_ETC1_CCZ = PVR2_ETC1 | CCZ,
		PVR3_ETC1_CCZ = PVR3_ETC1 | CCZ,
		PVR3_ETC2_CCZ = PVR3_ETC2 | CCZ,
		PVR_CCZ = PVR | CCZ,
		FileFormatMask = PVR_CCZ
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
	static QByteArray PVR_CCZ_Format();

private:
	static bool pvrPixelTypeHasAlpha(quint64 pixelTypeId);

	static int stringToFormat(const QByteArray &str);
	static QByteArray formatToString(int format);
	static const QList<QByteArray> &supportedSubTypes();

	bool ensureScanned() const;
	bool scanDevice() const;
	bool writeTexture(const pvr::Texture &texture);
	bool writeTexture(const pvrtexture::CPVRTexture &texture);

	quint32 getCurrentMipLevel() const;
	quint32 getCurrentArrayIndex() const;
	quint32 getCurrentFaceIndex() const;

	static void QImageTextureCleanup(void *ptr);

	using TexturePtr = QSharedPointer<pvrtexture::CPVRTexture>;
	mutable TexturePtr mTexture;
	QSize mScaledSize;
	int mCompressionRatio;
	int mQuality;
	mutable int mOrientation;
	mutable int mFormat;
	mutable int mImageFormat;
	mutable int mImageCurrentIndex;
	mutable bool mScanned;
};
