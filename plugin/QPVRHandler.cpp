#include "QPVRHandler.h"

#include <QVariant>
#include <QImage>

#include "PVRCore/Texture.h"
#include "PVRAssets/FileIO/TextureReaderPVR.h"
#include "PVRAssets/FileIO/TextureWriterPVR.h"
#include "PVRAssets/FileIO/TextureWriterLegacyPVR.h"

#include "QIODevicePVRAdapter.h"
#include "PVRTTexture.h"
#include "PVRTextureFormat.h"
#include "PVRTextureUtilities.h"

#include <qlogging.h>

using namespace pvrtexture;
using namespace pvr;
using namespace assets;
using namespace assetReaders;
using namespace assetWriters;

static const int SUPPORTED_FORMATS[] = {
	QPVRHandler::PVR2,
	QPVRHandler::PVR3,
	QPVRHandler::PVR2_PVRTC1_2,
	QPVRHandler::PVR3_PVRTC1_2,
	QPVRHandler::PVR2_PVRTC1_4,
	QPVRHandler::PVR3_PVRTC1_4,
	QPVRHandler::PVR2_PVRTC2_2,
	QPVRHandler::PVR3_PVRTC2_2,
	QPVRHandler::PVR2_PVRTC2_4,
	QPVRHandler::PVR3_PVRTC2_4,
	QPVRHandler::PVR2_ETC1,
	QPVRHandler::PVR3_ETC1,
	QPVRHandler::PVR3_ETC2,
};

static const int SUPPORTED_IMAGE_FORMATS[] = {
	QImage::Format_RGB32,
	QImage::Format_ARGB32_Premultiplied,
	QImage::Format_ARGB32,
	QImage::Format_RGB16,
	QImage::Format_RGB888,
	QImage::Format_ARGB4444_Premultiplied,
	QImage::Format_RGBX8888,
	QImage::Format_RGBA8888_Premultiplied,
	QImage::Format_RGBA8888,
	QImage::Format_Alpha8,
	QImage::Format_Grayscale8,
};

enum
{
	SUPPORTED_PFMT_COUNT = 10
};

static const int SUPPORTED_IMAGEFMT_PREMULT[SUPPORTED_PFMT_COUNT] = {
	QImage::Format_Invalid,
	QImage::Format_ARGB32_Premultiplied,
	QImage::Format_Invalid,
	QImage::Format_Invalid,
	QImage::Format_ARGB4444_Premultiplied,
	QImage::Format_Invalid,
	QImage::Format_RGBA8888_Premultiplied,
	QImage::Format_Invalid,
	QImage::Format_Invalid,
};

static const int SUPPORTED_IMAGEFMT_UNPREMULT[SUPPORTED_PFMT_COUNT] = {
	QImage::Format_RGB32,
	QImage::Format_ARGB32,
	QImage::Format_RGB16,
	QImage::Format_RGB888,
	QImage::Format_Invalid,
	QImage::Format_RGBX8888,
	QImage::Format_RGBA8888,
	QImage::Format_Alpha8,
	QImage::Format_Grayscale8,
};

static const PixelType SUPPORTED_PIXEL_TYPES[SUPPORTED_PFMT_COUNT] = {
	PixelType('b', 'g', 'r', 'x', 8, 8, 8, 8),
	PixelType('b', 'g', 'r', 'a', 8, 8, 8, 8),
	PixelType('r', 'g', 'b', 0, 5, 6, 5, 0),
	PixelType('r', 'g', 'b', 0, 8, 8, 8, 0),
	PixelType('a', 'r', 'g', 'b', 4, 4, 4, 4),
	PixelType('r', 'g', 'b', 'x', 8, 8, 8, 8),
	PixelType('r', 'g', 'b', 'a', 8, 8, 8, 8),
	PixelType('a', 0, 0, 0, 8, 0, 0, 0),
	PixelType('l', 0, 0, 0, 8, 0, 0, 0),
};

static const QByteArray s_premultiplied = QByteArrayLiteral("premultiplied");

QPVRHandler::QPVRHandler()
	: mQuality(-1)
	, mOrientation(0)
	, mFormat(UnknownFormat)
	, mImageFormat(QImage::Format_Invalid)
	, mImageCurrentIndex(-1)
	, mScanned(false)
{
}

QPVRHandler::Format QPVRHandler::detectFileFormat(QIODevice *device)
{
	Q_ASSERT(nullptr != device);
	Q_ASSERT(device->isOpen());

	Format result = UnknownFormat;

	device->startTransaction();
	{
		QIODevicePVRAdapter stream(device);

		if (TextureReaderPVR().isSupportedFile(stream))
		{
			result = PVR;
		}
	}
	device->rollbackTransaction();

	return result;
}

QByteArray QPVRHandler::PVR_Format()
{
	return QByteArrayLiteral("pvr");
}

bool QPVRHandler::canRead() const
{
	if (nullptr != mTexture)
		return true;

	mFormat = detectFileFormat(device());
	Q_ASSERT(0 == (mFormat & ~FileFormatMask));

	switch (mFormat)
	{
		case PVR:
			setFormat(PVR_Format());
			return true;

		default:
			break;
	}

	return false;
}

void QPVRHandler::QImageTextureCleanup(void *ptr)
{
	auto texturePtr = reinterpret_cast<TexturePtr *>(ptr);
	delete texturePtr;
}

bool QPVRHandler::read(QImage *image)
{
	if (!ensureScanned())
		return false;

	Q_ASSERT(nullptr != image);

	quint32 mipLevel = getCurrentMipLevel();
	quint32 arrayIndex = getCurrentArrayIndex();
	quint32 faceIndex = getCurrentFaceIndex();

	int width = int(mTexture->getWidth(mipLevel));
	int height = int(mTexture->getHeight(mipLevel));
	int bytesPerLine =
		int(mTexture->getDataSize(mipLevel, false, false) / quint32(height));

	*image = QImage(
		(const uchar *) mTexture->getDataPtr(mipLevel, arrayIndex, faceIndex),
		width, height, bytesPerLine, QImage::Format(mImageFormat),
		&QPVRHandler::QImageTextureCleanup, new TexturePtr(mTexture));

	if (mScaledSize.isValid() && !mScaledSize.isNull() &&
		mScaledSize != image->size())
	{
		*image = image->scaled(
			mScaledSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	}

	if (image->format() != QImage::Format(mImageFormat))
		*image = image->convertToFormat(QImage::Format(mImageFormat));
	return true;
}

static int pvrPixelTypeToImageFormat(quint64 pixelTypeId, bool premultiplied)
{
	for (int i = 0; i < SUPPORTED_PFMT_COUNT; i++)
	{
		if (pixelTypeId == SUPPORTED_PIXEL_TYPES[i].PixelTypeID)
		{
			if (premultiplied)
				return SUPPORTED_IMAGEFMT_PREMULT[i];
			return SUPPORTED_IMAGEFMT_UNPREMULT[i];
		}
	}

	return QImage::Format_Invalid;
}

static bool imageFormatToPvrPixelType(
	int format, PixelType *pixelTypePtr, bool *premultipliedPtr)
{
	if (format == QImage::Format_Invalid)
		return false;

	PixelType pixelType;
	bool premultiplied = false;
	bool found = false;
	for (int i = 0; i < SUPPORTED_PFMT_COUNT; i++)
	{
		if (format == SUPPORTED_IMAGEFMT_PREMULT[i])
		{
			pixelType = SUPPORTED_PIXEL_TYPES[i];
			premultiplied = true;
			found = true;
			break;
		}
		if (format == SUPPORTED_IMAGEFMT_UNPREMULT[i])
		{
			pixelType = SUPPORTED_PIXEL_TYPES[i];
			found = true;
			break;
		}
	}

	if (!found)
		return false;

	if (nullptr != pixelTypePtr)
		*pixelTypePtr = pixelType;

	if (nullptr != premultipliedPtr)
		*premultipliedPtr = premultiplied;

	return true;
}

static int convertQuality(int q, int total, int qdefault)
{
	return q < 0 ? qdefault
				 : (qMin(q, 100) * (total - 1)) / (100 - (100 / total - 1));
}

bool QPVRHandler::write(const QImage &image)
{
	if (mFormat == UnknownFormat)
	{
		mFormat = PVR3;
	}

	PixelType pixelType;
	bool isPremultiplied = false;
	QImage img = image;

	auto imageFormat = QImage::Format(mImageFormat);
	if (imageFormat == QImage::Format_Invalid)
	{
		imageFormat = image.format();
	}

	switch (imageFormat)
	{
		case QImage::Format_RGB555:
			imageFormat = QImage::Format_RGB16;
			break;

		case QImage::Format_RGB444:
			imageFormat = QImage::Format_ARGB4444_Premultiplied;
			break;

		default:
			break;
	}

	if (!imageFormatToPvrPixelType(imageFormat, &pixelType, &isPremultiplied))
	{
		if (image.hasAlphaChannel())
		{
			imageFormat = image.pixelFormat().premultiplied() ==
					QPixelFormat::Premultiplied
				? QImage::Format_RGBA8888_Premultiplied
				: QImage::Format_RGBA8888;
		} else
		{
			imageFormat = QImage::Format_RGB888;
		}

		if (!imageFormatToPvrPixelType(
				imageFormat, &pixelType, &isPremultiplied))
			return false;
	}

	if (img.format() != imageFormat)
	{
		img = image.convertToFormat(imageFormat);
	}

	int pixelSize = (img.pixelFormat().bitsPerPixel() + 7) / 8;
	int width = img.width();
	int height = img.height();

	CPVRTextureHeader header(pixelType.PixelTypeID, height, width, 1, 1, 1, 1,
		ePVRTCSpacelRGB, ePVRTVarTypeUnsignedByteNorm, isPremultiplied);

	header.setOrientation(EPVRTOrientation(mOrientation));

	CPVRTexture texture(header);

	int widthBytes = img.width() * pixelSize;
	int pvrWidthBytes =
		int(header.getDataSize(PVRTEX_TOPMIPLEVEL, false, false)) /
		quint32(img.height());
	auto pvrPtr = reinterpret_cast<char *>(texture.getDataPtr());

	for (int y = 0; y < height; y++)
	{
		memcpy(pvrPtr, img.scanLine(y), widthBytes);
		pvrPtr += pvrWidthBytes;
	}

	img = QImage();

	bool texV2 = 0 != (mFormat & TEXV2);
	bool tex4bpp = 0 != (mFormat & TEXBIT4);

	PixelType outputPixelType = pixelType;
	ECompressorQuality quality = ePVRTCFastest;

	if (0 != (mFormat & ETC))
	{
		switch (convertQuality(mQuality, 2, 0))
		{
			default:
				quality = eETCFastPerceptual;
				break;

			case 1:
				quality = eETCSlowPerceptual;
				break;
		}

		if (texV2)
		{
			if (pvrPixelTypeHasAlpha(outputPixelType.PixelTypeID))
			{
				outputPixelType = ePVRTPF_ETC2_RGBA;
			} else
			{
				outputPixelType = ePVRTPF_ETC2_RGB;
			}
		} else
		{
			outputPixelType = ePVRTPF_ETC1;
		}
	} else if (0 != (mFormat & PVRTC))
	{
		quality = ECompressorQuality(
			convertQuality(mQuality, eNumPVRTCModes, ePVRTCNormal));

		if (texV2)
		{
			outputPixelType =
				tex4bpp ? ePVRTPF_PVRTCII_4bpp : ePVRTPF_PVRTCII_2bpp;
		} else if (pvrPixelTypeHasAlpha(outputPixelType.PixelTypeID))
		{
			outputPixelType =
				tex4bpp ? ePVRTPF_PVRTCI_4bpp_RGBA : ePVRTPF_PVRTCI_2bpp_RGBA;
		} else
		{
			outputPixelType =
				tex4bpp ? ePVRTPF_PVRTCI_4bpp_RGB : ePVRTPF_PVRTCI_2bpp_RGB;
		}
	}

	if (pixelType.PixelTypeID == outputPixelType.PixelTypeID ||
		Transcode(texture, outputPixelType, texture.getChannelType(),
			texture.getColourSpace(), quality))
	{
		return writeTexture(texture);
	}

	return false;
}

QVariant QPVRHandler::option(ImageOption option) const
{
	if (supportsOption(option))
	{
		switch (option)
		{
			case SubType:
				ensureScanned();
				return subType(mFormat, mImageFormat);

			case SupportedSubTypes:
			{
				return QVariant::fromValue<QList<QByteArray>>(
					supportedSubTypes());
			}

			case Size:
			{
				if (!ensureScanned())
					break;

				return QSize(mTexture->getWidth(), mTexture->getHeight());
			}

			case ScaledSize:
				return mScaledSize;

			case Quality:
				return mQuality;

			case ImageFormat:
				ensureScanned();
				return mImageFormat;

			case ImageTransformation:
			{
				ensureScanned();

				Transformations t;

				if (ePVRTOrientLeft == (ePVRTOrientLeft & mOrientation))
				{
					t |= TransformationMirror;
				}

				if (ePVRTOrientUp == (ePVRTOrientUp & mOrientation))
				{
					t |= TransformationFlip;
				}

				return int(t);
			}

			default:
				break;
		}
	}

	return QVariant();
}

void QPVRHandler::setOption(ImageOption option, const QVariant &value)
{
	bool ok = false;

	switch (option)
	{
		case SubType:
		{
			int format = value.toInt(&ok);
			int imageFormat = QImage::Format_Invalid;
			if (ok)
			{
				ok = false;
				for (int f : SUPPORTED_FORMATS)
				{
					if (f == format)
					{
						ok = true;
						break;
					}
				}
			} else
			{
				ok = stringToFormat(
					value.toString().toLatin1(), &format, &imageFormat);
			}

			if (ok)
			{
				mFormat = format;
				mImageFormat = imageFormat;
			} else
			{
				qWarning("Unknown sub type '%s'.",
					value.toString().toLatin1().data());
			}
			break;
		}

		case ScaledSize:
			mScaledSize = value.toSize();
			break;

		case Quality:
		{
			int q = value.toInt(&ok);

			mQuality = ok ? q : -1;
			break;
		}

		case ImageFormat:
		{
			int fmt = value.toInt(&ok);
			Q_ASSERT(ok);
			Q_ASSERT(fmt > QImage::Format_Invalid);
			Q_ASSERT(fmt < QImage::NImageFormats);

			mImageFormat = fmt;
			break;
		}

		case ImageTransformation:
		{
			Transformations t(value.toInt(&ok));
			Q_ASSERT(ok);

			int hor = (0 != (t & TransformationMirror))
				? TextureMetaData::AxisOrientationLeft
				: TextureMetaData::AxisOrientationRight;

			int ver = (0 != (t & TransformationFlip))
				? TextureMetaData::AxisOrientationUp
				: TextureMetaData::AxisOrientationDown;

			if (0 != (t & TransformationRotate90))
			{
				qWarning("PVR rotation is not supported.");
			}

			mOrientation = hor | ver;
			break;
		}

		default:
			break;
	}
}

bool QPVRHandler::supportsOption(ImageOption option) const
{
	switch (option)
	{
		case SubType:
		case SupportedSubTypes:
		case Size:
		case ScaledSize:
		case Quality:
		case ImageFormat:
		case ImageTransformation:
			return true;

		default:
			break;
	}

	return false;
}

bool QPVRHandler::jumpToImage(int imageNumber)
{
	if (imageNumber < 0 || imageNumber >= imageCount())
		return false;

	mImageCurrentIndex = imageNumber;
	return false;
}

bool QPVRHandler::jumpToNextImage()
{
	return jumpToImage(mImageCurrentIndex + 1);
}

bool QPVRHandler::pvrPixelTypeHasAlpha(quint64 pixelTypeId)
{
	PixelFormat pixelFormat(pixelTypeId);

	if (pixelFormat.isCompressedFormat())
	{
		switch (CompressedPixelFormat(pixelTypeId))
		{
			case CompressedPixelFormat::PVRTCI_2bpp_RGBA:
			case CompressedPixelFormat::PVRTCI_4bpp_RGBA:
			case CompressedPixelFormat::PVRTCII_2bpp:
			case CompressedPixelFormat::PVRTCII_4bpp:
			case CompressedPixelFormat::ETC2_RGBA:
			case CompressedPixelFormat::ETC2_RGB_A1:
			case CompressedPixelFormat::DXT1:
			case CompressedPixelFormat::DXT2:
			case CompressedPixelFormat::DXT3:
			case CompressedPixelFormat::DXT4:
			case CompressedPixelFormat::DXT5:
				return true;

			default:
				break;
		}
	} else
	{
		if (pixelFormat == PixelFormat::Intensity8)
			return true;

		for (quint8 i = 0; i < 4; i++)
		{
			if (pixelFormat.getChannelContent(i) == 'a')
			{
				return true;
			}
		}
	}

	return false;
}

bool QPVRHandler::stringToFormat(
	const QByteArray &str, int *format, int *imageFormat)
{
	QList<QByteArray> split = str.split('.');

	if (split.isEmpty())
		return false;

	static const QByteArray s_pvr = QByteArrayLiteral("pvr");
	static const QByteArray s_pvrtc = QByteArrayLiteral("pvrtc");
	static const QByteArray s_etc = QByteArrayLiteral("etc");

	auto pvr = split.takeFirst();

	if (pvr.length() != s_pvr.length() + 1 || !pvr.startsWith(s_pvr))
		return false;

	int result = 0;

	switch (pvr.at(3))
	{
		case '2':
			result |= PVR2;
			break;

		case '3':
			result |= PVR3;
			break;

		default:
			return 0;
	}

	if (split.isEmpty())
		return result;

	auto s = split.takeFirst();

	if (split.length() <= 1)
	{
		PixelType pt;

		int cnt = 0;
		for (int i = 0, count = std::min(4, s.length()); i < count; i++)
		{
			quint8 c = quint8(s.at(i));
			switch (c)
			{
				case 'r':
				case 'g':
				case 'b':
				case 'a':
				case 'x':
				case 'i':
				case 'l':
					pt.PixelTypeChar[i] = quint8(c);
					cnt++;
					break;
				default:
					c = 0;
					break;
			}
			if (c == 0)
				break;
		}
		switch (cnt)
		{
			case 1:
			case 3:
			case 4:
			{
				for (int i = cnt, k = 4, count = s.length(); i < count;
					 i++, k++)
				{
					char c = s.at(i);
					switch (c)
					{
						case '4':
						case '5':
						case '6':
						case '8':
							pt.PixelTypeChar[k] = quint8(c - '0');
							cnt++;
							break;
						default:
							c = 0;
							break;
					}
					if (c == 0)
					{
						cnt = -1;
						break;
					}
				}
				break;
			}

			default:
				cnt = -1;
				break;
		}
		if (cnt == s.length())
		{
			int iFmt = pvrPixelTypeToImageFormat(pt.PixelTypeID,
				!split.isEmpty() && split.at(0) == s_premultiplied);
			if (iFmt != QImage::Format_Invalid)
			{
				if (format)
					*format = result;
				if (imageFormat)
					*imageFormat = iFmt;
				return true;
			}
		}
	}

	int skip;
	int suffix;

	if (s.startsWith(s_pvrtc))
	{
		result |= PVRTC;
		skip = s_pvrtc.length();
		suffix = 3;
	} else if (s.startsWith(s_etc))
	{
		result |= ETC;
		skip = s_etc.length();
		suffix = 1;
	} else
	{
		return 0;
	}

	if (s.length() != skip + suffix)
	{
		return 0;
	}

	switch (s.at(skip))
	{
		case '1':
			break;

		case '2':
			result |= TEXV2;
			break;

		default:
			return 0;
	}

	if (suffix >= 3)
	{
		switch (s.at(skip + 2))
		{
			case '2':
				break;

			case '4':
				result |= TEXBIT4;
				break;

			default:
				return 0;
		}
	}

	if (!split.isEmpty())
	{
		s = split.takeFirst();
	}

	return result;
}

QByteArray QPVRHandler::formatToString(int format)
{
	QByteArray result;

	if (PVR3 == (format & PVR3))
	{
		result += QByteArrayLiteral("pvr3");
	} else if (PVR2 == (format & PVR2))
	{
		result += QByteArrayLiteral("pvr2");
	} else
	{
		return result;
	}

	bool compressed = false;
	bool isETC = 0 != (format & ETC);

	if (isETC)
	{
		result += QByteArrayLiteral(".etc");
		compressed = true;
	} else if (0 != (format & PVRTC))
	{
		result += QByteArrayLiteral(".pvrtc");
		compressed = true;
	}

	if (compressed)
	{
		result += (0 != (format & TEXV2)) ? '2' : '1';

		if (!isETC)
		{
			result += '_';
			result += (0 != (format & TEXBIT4)) ? '4' : '2';
		}
	}

	return result;
}

QByteArray QPVRHandler::subType(int format, int imageFormat)
{
	if (format == UnknownFormat ||
		(PVR3 != (format & PVR3) && PVR2 != (format & PVR2)))
	{
		format |= PVR3;
	}

	auto result = formatToString(format);
	switch (format)
	{
		case PVR2:
		case PVR3:
		{
			if (imageFormat == QImage::Format_Invalid)
				break;

			PixelType pt;
			bool premultiplied;
			bool ok =
				imageFormatToPvrPixelType(imageFormat, &pt, &premultiplied);
			Q_ASSERT(ok);

			result += '.';
			for (int i = 0; i < 4; i++)
			{
				uint c = pt.PixelTypeChar[i];
				if (c != 0)
					result += char(c);
			}
			for (int i = 4; i < 8; i++)
			{
				uint c = pt.PixelTypeChar[i];
				if (c != 0)
					result += QByteArray::number(c);
			}
			if (premultiplied)
				result += '.' + s_premultiplied;
			break;
		}

		default:
			break;
	}

	return result;
}

const QList<QByteArray> &QPVRHandler::supportedSubTypes()
{
	static QList<QByteArray> result;

	if (result.isEmpty())
	{
		for (int fmt : SUPPORTED_FORMATS)
		{
			result.append(formatToString(fmt));
			if (fmt == QPVRHandler::PVR2 || fmt == QPVRHandler::PVR3)
			{
				for (int iFmt : SUPPORTED_IMAGE_FORMATS)
				{
					result.append(subType(fmt, iFmt));
				}
			}
		}
	}

	return result;
}

bool QPVRHandler::ensureScanned() const
{
	if (!mScanned)
	{
		mScanned = true;
		return scanDevice();
	}

	return (nullptr != mTexture);
}

bool QPVRHandler::scanDevice() const
{
	if (!canRead())
		return false;

	Q_ASSERT(nullptr == mTexture);

	quint32 signature;
	device()->peek(reinterpret_cast<char *>(&signature), 4);

	Texture tex;
	bool readOk;
	{
		Stream::ptr_type stream(new QIODevicePVRAdapter(device()));
		TextureReaderPVR reader(stream);

		readOk = reader.readAsset(tex);
	} // destroy reader and stream adapter

	if (readOk)
	{
		auto pixelFormat = tex.getPixelFormat();
		PixelFormat cvtPixelFormat;

		bool isPremultiplied = tex.isPreMultiplied();

		mFormat &= FileFormatMask;

		if (pixelFormat.isCompressedFormat())
		{
			switch (CompressedPixelFormat(pixelFormat.getPixelTypeId()))
			{
				case CompressedPixelFormat::PVRTCII_2bpp:
				{
					mFormat |= TEXV2;
					// fall through
				}

				case CompressedPixelFormat::PVRTCI_2bpp_RGBA:
				case CompressedPixelFormat::PVRTCI_2bpp_RGB:
					mFormat |= PVRTC;
					break;

				case CompressedPixelFormat::PVRTCII_4bpp:
				{
					mFormat |= TEXV2;
					// fall through
				}

				case CompressedPixelFormat::PVRTCI_4bpp_RGBA:
				case CompressedPixelFormat::PVRTCI_4bpp_RGB:
					mFormat |= PVRTC | TEXBIT4;
					break;

				case CompressedPixelFormat::ETC2_RGBA:
				case CompressedPixelFormat::ETC2_RGB_A1:
				{
					mFormat |= TEXV2;
					// fall through
				}

				case CompressedPixelFormat::ETC1:
					mFormat |= ETC;
					break;

				default:
					break;
			}
			mImageFormat = QImage::Format_Invalid;
		} else
		{
			mImageFormat = pvrPixelTypeToImageFormat(
				pixelFormat.getPixelTypeId(), isPremultiplied);
		}

		if (mImageFormat == QImage::Format_Invalid)
		{
			if (pvrPixelTypeHasAlpha(pixelFormat.getPixelTypeId()))
			{
				cvtPixelFormat = PixelFormat::RGBA_8888;

				mImageFormat = isPremultiplied
					? QImage::Format_RGBA8888_Premultiplied
					: QImage::Format_RGBA8888;
			} else
			{
				cvtPixelFormat = PixelFormat::RGB_888;

				mImageFormat = QImage::Format_RGB888;
			}
		} else
		{
			cvtPixelFormat = pixelFormat;
		}

		CPVRTextureHeader header(pixelFormat.getPixelTypeId(), tex.getHeight(),
			tex.getWidth(), tex.getDepth(), tex.getNumberOfMIPLevels(),
			tex.getNumberOfArrayMembers(), tex.getNumberOfFaces(),
			(EPVRTColourSpace) tex.getColorSpace(),
			(EPVRTVariableType) tex.getChannelType(), isPremultiplied);

		int orientation = tex.getOrientation(TextureMetaData::AxisAxisX) |
			tex.getOrientation(TextureMetaData::AxisAxisY) |
			tex.getOrientation(TextureMetaData::AxisAxisZ);

		header.setOrientation((EPVRTOrientation) orientation);

		TexturePtr texture(new CPVRTexture(header, tex.getDataPointer()));

		if ((cvtPixelFormat == pixelFormat &&
				tex.getColorSpace() == types::ColorSpace::lRGB &&
				tex.getChannelType() == VariableType::UnsignedByteNorm) ||
			Transcode(*texture.data(), cvtPixelFormat.getPixelTypeId(),
				ePVRTVarTypeUnsignedByteNorm, ePVRTCSpacelRGB,
				ECompressorQuality(0)))
		{
			mFormat |=
				(signature == TextureHeader::Header::PVRv3) ? PVR3 : PVR2;
			mImageCurrentIndex = 0;
			mOrientation = orientation;
			mTexture = texture;
			return true;
		}
	}

	return false;
}

bool QPVRHandler::writeTexture(const Texture &texture)
{
	std::unique_ptr<AssetWriter<Texture>> writerPtr;

	if (PVR3 == (mFormat & PVR3))
	{
		writerPtr.reset(new TextureWriterPVR);
	} else
	{
		auto legacyWriter = new TextureWriterLegacyPVR;
		legacyWriter->setTargetAPI(texture_legacy::ApiOGL);
		writerPtr.reset(legacyWriter);
	}

	AssetWriter<Texture> *writer = writerPtr.get();

	if (!writer->canWriteAsset(texture) || !writer->addAssetToWrite(texture))
		return false;

	bool result;
	{
		Stream::ptr_type stream(new QIODevicePVRAdapter(device()));
		result = writer->openAssetStream(stream) && writer->writeAllAssets();
		writerPtr.reset(nullptr);
	}

	return result;
}

bool QPVRHandler::writeTexture(const CPVRTexture &texture)
{
	auto &oldHeader = texture.getHeader();

	TextureHeader header(oldHeader.getPixelType().PixelTypeID,
		oldHeader.getWidth(), oldHeader.getHeight(), oldHeader.getDepth(),
		oldHeader.getNumMIPLevels(),
		types::ColorSpace(oldHeader.getColourSpace()),
		VariableType(oldHeader.getChannelType()),
		oldHeader.getNumArrayMembers(), oldHeader.getNumFaces());

	header.setOrientation(
		TextureMetaData::AxisOrientation(oldHeader.getOrientation(ePVRTAxisX) |
			oldHeader.getOrientation(ePVRTAxisY) |
			oldHeader.getOrientation(ePVRTAxisZ)));
	auto cubeMapOrder = oldHeader.getCubeMapOrder();

	if (!cubeMapOrder.empty())
		header.setCubeMapOrder(cubeMapOrder.c_str());

	header.setIsPreMultiplied(oldHeader.isPreMultiplied());

	return writeTexture(Texture(header, (const quint8 *) texture.getDataPtr()));
}

quint32 QPVRHandler::getCurrentMipLevel() const
{
	if (!mScaledSize.isValid() || mScaledSize.isNull())
		return PVRTEX_TOPMIPLEVEL;

	auto lastMipLevel = PVRTEX_TOPMIPLEVEL;
	lastMipLevel += mTexture->getNumMIPLevels();

	for (auto level = PVRTEX_TOPMIPLEVEL; level < lastMipLevel; level++)
	{
		if (mTexture->getWidth(level) == uint(mScaledSize.width()) &&
			mTexture->getHeight(level) == uint(mScaledSize.height()))
		{
			return level;
		}
	}

	return PVRTEX_TOPMIPLEVEL;
}

quint32 QPVRHandler::getCurrentArrayIndex() const
{
	quint32 curIndex = quint32(mImageCurrentIndex);
	quint32 numFaces = mTexture->getNumFaces();
	return curIndex / numFaces;
}

quint32 QPVRHandler::getCurrentFaceIndex() const
{
	quint32 curIndex = quint32(mImageCurrentIndex);
	quint32 numFaces = mTexture->getNumFaces();
	return curIndex % numFaces;
}

int QPVRHandler::imageCount() const
{
	if (!ensureScanned())
		return 0;

	auto &header = mTexture->getHeader();

	return header.getNumArrayMembers() * header.getNumFaces();
}

int QPVRHandler::currentImageNumber() const
{
	return mImageCurrentIndex;
}
