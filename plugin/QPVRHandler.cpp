#include "QPVRHandler.h"

#include <QVariant>
#include <QImage>

#include "PVRCore/Texture.h"
#include "PVRAssets/FileIO/TextureReaderPVR.h"
#include "PVRAssets/FileIO/TextureWriterPVR.h"
#include "PVRAssets/FileIO/TextureWriterLegacyPVR.h"

#include "QIODevicePVRAdapter.h"
#include "PVRTextureWrapper.h"

#include <qlogging.h>

static const int SUPPORTED_FORMATS[] = {
	QPVRHandler::PVR2, //
	QPVRHandler::PVR3, //
	QPVRHandler::PVR2_PVRTC1_2, //
	QPVRHandler::PVR3_PVRTC1_2, //
	QPVRHandler::PVR2_PVRTC1_4, //
	QPVRHandler::PVR3_PVRTC1_4, //
	QPVRHandler::PVR2_PVRTC2_2, //
	QPVRHandler::PVR3_PVRTC2_2, //
	QPVRHandler::PVR2_PVRTC2_4, //
	QPVRHandler::PVR3_PVRTC2_4, //
	QPVRHandler::PVR2_ETC1, //
	QPVRHandler::PVR3_ETC1, //
	QPVRHandler::PVR3_ETC2, //
};

using namespace pvr;
using namespace assets;
using namespace assetReaders;
using namespace assetWriters;

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

	*image = QImage((const uchar *) mTexture->getDataPointer(
						mipLevel, arrayIndex, faceIndex),
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

static bool imageFormatToPvrPixelType(
	int format, PixelFormat *pixelTypePtr, bool *premultipliedPtr)
{
	PixelFormat pixelType;
	bool premultiplied = false;

	switch (format)
	{
		case QImage::Format_RGB32:
			pixelType = PixelFormat('b', 'g', 'r', 'x', 8, 8, 8, 8);
			break;

		case QImage::Format_ARGB32_Premultiplied:
		{
			premultiplied = true;
			// fall through
		}

		case QImage::Format_ARGB32:
			pixelType = PixelFormat('b', 'g', 'r', 'a', 8, 8, 8, 8);
			break;

		case QImage::Format_RGB16:
			pixelType = PixelFormat('r', 'g', 'b', 0, 5, 6, 5, 0);
			break;

		case QImage::Format_RGB888:
			pixelType = PixelFormat('r', 'g', 'b', 0, 8, 8, 8, 0);
			break;

		case QImage::Format_ARGB4444_Premultiplied:
			premultiplied = true;
			pixelType = PixelFormat('a', 'r', 'g', 'b', 4, 4, 4, 4);
			break;

		case QImage::Format_RGBX8888:
			pixelType = PixelFormat('r', 'g', 'b', 'x', 8, 8, 8, 8);
			break;

		case QImage::Format_RGBA8888_Premultiplied:
			premultiplied = true;

		case QImage::Format_RGBA8888:
			pixelType = PixelFormat('r', 'g', 'b', 'a', 8, 8, 8, 8);
			break;

		case QImage::Format_Alpha8:
			pixelType = PixelFormat('a', 0, 0, 0, 8, 0, 0, 0);
			break;

		case QImage::Format_Grayscale8:
			pixelType = PixelFormat('i', 0, 0, 0, 8, 0, 0, 0);
			break;

		default:
			return false;
	}

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

	PixelFormat pixelType;
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

	PVRTextureWrapper texture(
		pixelType, width, height, mOrientation, isPremultiplied);

	int widthBytes = img.width() * pixelSize;
	int pvrWidthBytes =
		int(texture.getDataSize(0, false, false)) / quint32(img.height());
	auto pvrPtr = reinterpret_cast<char *>(texture.getDataPointer());

	for (int y = 0; y < height; y++)
	{
		memcpy(pvrPtr, img.scanLine(y), widthBytes);
		pvrPtr += pvrWidthBytes;
	}

	img = QImage();

	bool texV2 = 0 != (mFormat & TEXV2);
	bool tex4bpp = 0 != (mFormat & TEXBIT4);

	PixelFormat outputPixelType = pixelType;
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
			if (pvrPixelTypeHasAlpha(outputPixelType.getPixelTypeId()))
			{
				outputPixelType = CompressedPixelFormat::ETC2_RGBA;
			} else
			{
				outputPixelType = CompressedPixelFormat::ETC2_RGB;
			}
		} else
		{
			outputPixelType = CompressedPixelFormat::ETC1;
		}
	} else if (0 != (mFormat & PVRTC))
	{
		quality = ECompressorQuality(
			convertQuality(mQuality, eNumPVRTCModes, ePVRTCNormal));

		if (texV2)
		{
			outputPixelType = tex4bpp ? CompressedPixelFormat::PVRTCII_4bpp
									  : CompressedPixelFormat::PVRTCII_2bpp;
		} else if (pvrPixelTypeHasAlpha(outputPixelType.getPixelTypeId()))
		{
			outputPixelType = tex4bpp ? CompressedPixelFormat::PVRTCI_4bpp_RGBA
									  : CompressedPixelFormat::PVRTCI_2bpp_RGBA;
		} else
		{
			outputPixelType = tex4bpp ? CompressedPixelFormat::PVRTCI_4bpp_RGB
									  : CompressedPixelFormat::PVRTCI_2bpp_RGB;
		}
	}

	if (texture.transcode(outputPixelType, quality))
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
				return formatToString(mFormat);

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

				if (TextureMetaData::AxisOrientationLeft ==
					(TextureMetaData::AxisOrientationLeft & mOrientation))
				{
					t |= TransformationMirror;
				}

				if (TextureMetaData::AxisOrientationUp ==
					(TextureMetaData::AxisOrientationUp & mOrientation))
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

			if (!ok)
				format = stringToFormat(value.toString().toLatin1());

			for (int f : SUPPORTED_FORMATS)
			{
				if (f == format)
				{
					mFormat = format;
					return;
				}
			}

			qWarning(
				"Unknown sub type '%s'.", value.toString().toLatin1().data());
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

int QPVRHandler::stringToFormat(const QByteArray &str)
{
	QList<QByteArray> split = str.split('.');

	if (split.isEmpty())
		return 0;

	static const QByteArray s_pvr = QByteArrayLiteral("pvr");
	static const QByteArray s_pvrtc = QByteArrayLiteral("pvrtc");
	static const QByteArray s_etc = QByteArrayLiteral("etc");

	auto pvr = split.takeFirst();

	if (pvr.length() != s_pvr.length() + 1 || !pvr.startsWith(s_pvr))
		return 0;

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

const QList<QByteArray> &QPVRHandler::supportedSubTypes()
{
	static QList<QByteArray> result;

	if (result.isEmpty())
	{
		for (int fmt : SUPPORTED_FORMATS)
		{
			result.append(formatToString(fmt));
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

	TexturePtr texture(new PVRTextureWrapper);
	bool readOk;
	{
		Stream::ptr_type stream(new QIODevicePVRAdapter(device()));
		TextureReaderPVR reader(stream);

		readOk = reader.readAsset(*texture.data());
	} // destroy reader and stream adapter

	if (readOk)
	{
		auto pixelFormat = texture->getPixelFormat();
		PixelFormat cvtPixelFormat;

		bool isPremultiplied = texture->isPreMultiplied();

		mFormat &= FileFormatMask;

		if (pixelFormat == PixelFormat::RGB_565)
		{
			cvtPixelFormat = pixelFormat;

			mImageFormat = QImage::Format_RGB16;
		} else if (pixelFormat == PixelFormat::Intensity8)
		{
			cvtPixelFormat = pixelFormat;

			mImageFormat = QImage::Format_Grayscale8;
		} else
		{
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
			}

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
		}

		if (texture->transcode(cvtPixelFormat, ECompressorQuality(0)))
		{
			mFormat |=
				(signature == TextureHeader::Header::PVRv3) ? PVR3 : PVR2;
			mImageCurrentIndex = 0;
			mOrientation = texture->getOrientation(TextureMetaData::AxisAxisX) |
				texture->getOrientation(TextureMetaData::AxisAxisY) |
				texture->getOrientation(TextureMetaData::AxisAxisZ);
			mTexture = texture;
			return true;
		}
	}

	return false;
}

bool QPVRHandler::writeTexture(const PVRTextureWrapper &texture)
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

quint32 QPVRHandler::getCurrentMipLevel() const
{
	if (!mScaledSize.isValid() || mScaledSize.isNull())
		return 0;

	auto lastMipLevel = 0;
	lastMipLevel += mTexture->getNumberOfMIPLevels();

	for (auto level = 0; level < lastMipLevel; level++)
	{
		if (mTexture->getWidth(level) == uint(mScaledSize.width()) &&
			mTexture->getHeight(level) == uint(mScaledSize.height()))
		{
			return level;
		}
	}

	return 0;
}

quint32 QPVRHandler::getCurrentArrayIndex() const
{
	quint32 curIndex = quint32(mImageCurrentIndex);
	quint32 numFaces = mTexture->getNumberOfFaces();
	return curIndex / numFaces;
}

quint32 QPVRHandler::getCurrentFaceIndex() const
{
	quint32 curIndex = quint32(mImageCurrentIndex);
	quint32 numFaces = mTexture->getNumberOfArrayMembers();
	return curIndex % numFaces;
}

int QPVRHandler::imageCount() const
{
	if (!ensureScanned())
		return 0;

	return mTexture->getNumberOfArrayMembers() * mTexture->getNumberOfFaces();
}

int QPVRHandler::currentImageNumber() const
{
	return mImageCurrentIndex;
}
