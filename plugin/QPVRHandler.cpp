#include "QPVRHandler.h"

#include <QVariant>
#include <QImage>

#include "PVRAssets/Texture/Texture.h"
#include "PVRAssets/FileIO/TextureReaderPVR.h"
#include "PVRAssets/FileIO/TextureWriterPVR.h"
#include "PVRAssets/FileIO/TextureWriterLegacyPVR.h"

#include "QIODevicePVRAdapter.h"
#include "PVRTTexture.h"
#include "PVRTextureFormat.h"
#include "PVRTextureUtilities.h"

#include <QDebug>

#include "QCCZStream.h"
#undef compress

static const int SUPPORTED_FORMATS[] = {
	QPVRHandler::PVR2,
	QPVRHandler::PVR3,
	QPVRHandler::PVR2_CCZ,
	QPVRHandler::PVR3_CCZ,
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
	QPVRHandler::PVR2_PVRTC1_2_CCZ,
	QPVRHandler::PVR3_PVRTC1_2_CCZ,
	QPVRHandler::PVR2_PVRTC1_4_CCZ,
	QPVRHandler::PVR3_PVRTC1_4_CCZ,
	QPVRHandler::PVR2_PVRTC2_2_CCZ,
	QPVRHandler::PVR3_PVRTC2_2_CCZ,
	QPVRHandler::PVR2_PVRTC2_4_CCZ,
	QPVRHandler::PVR3_PVRTC2_4_CCZ,
	QPVRHandler::PVR2_ETC1_CCZ,
	QPVRHandler::PVR3_ETC1_CCZ,
	QPVRHandler::PVR3_ETC2_CCZ
};

using namespace pvrtexture;
using namespace pvr;
using namespace assets;
using namespace assetReaders;
using namespace assetWriters;

QPVRHandler::QPVRHandler()
	: mCompressionRatio(Z_DEFAULT_COMPRESSION)
	, mQuality(1.0)
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

	auto origDevice = device;
	origDevice->startTransaction();
	{
		bool ccz = false;
		QCCZDecompressionStream decompressor(origDevice);
		if (decompressor.open())
		{
			device = &decompressor;
			ccz = true;
		} else
		{
			origDevice->rollbackTransaction();
			origDevice->startTransaction();
		}

		QIODevicePVRAdapter stream(device);

		if (TextureReaderPVR().isSupportedFile(stream))
		{
			result = ccz ? PVR_CCZ : PVR;
		}
	}
	origDevice->rollbackTransaction();

	return result;
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
			setFormat(QByteArrayLiteral("pvr"));
			return true;

		case PVR_CCZ:
			setFormat(QByteArrayLiteral("pvr.ccz"));
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

	int width = static_cast<int>(mTexture->getWidth(mipLevel));
	int height = static_cast<int>(mTexture->getHeight(mipLevel));
	int bytesPerLine = static_cast<int>(
			mTexture->getDataSize(mipLevel, false, false) /
			static_cast<quint32>(height));

	*image = QImage(
			(const uchar *) mTexture->getDataPtr(
				mipLevel, arrayIndex, faceIndex),
			width, height, bytesPerLine,
			(QImage::Format) mImageFormat,
			&QPVRHandler::QImageTextureCleanup,
			new TexturePtr(mTexture));

	if (mScaledSize.isValid() &&
		!mScaledSize.isNull() &&
		mScaledSize != image->size())
	{
		*image = image->scaled(
				mScaledSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)
			.convertToFormat((QImage::Format) mImageFormat);
	}

	return true;
}

static bool imageFormatToPvrPixelType(
	int format, PixelType *pixelTypePtr, bool *premultipliedPtr)
{
	PixelType pixelType;
	bool premultiplied = false;

	switch (format)
	{
		case QImage::Format_RGB32:
			pixelType = PixelType('b', 'g', 'r', 'x', 8, 8, 8, 8);
			break;

		case QImage::Format_ARGB32_Premultiplied:
			premultiplied = true;

		case QImage::Format_ARGB32:
			pixelType = PixelType('b', 'g', 'r', 'a', 8, 8, 8, 8);
			break;

		case QImage::Format_RGB16:
			pixelType = PixelType('r', 'g', 'b', 0, 5, 6, 5, 0);
			break;

		case QImage::Format_ARGB8565_Premultiplied:
			premultiplied = true;
			pixelType = PixelType('a', 'r', 'g', 'b', 8, 5, 6, 5);
			break;

		case QImage::Format_ARGB8555_Premultiplied:
			premultiplied = true;
			pixelType = PixelType('a', 'r', 'g', 'b', 8, 5, 5, 5);
			break;

		case QImage::Format_RGB888:
			pixelType = PixelType('r', 'g', 'b', 0, 8, 8, 8, 0);
			break;

		case QImage::Format_RGB444:
			pixelType = PixelType('r', 'g', 'b', 0, 4, 4, 4, 0);
			break;

		case QImage::Format_ARGB4444_Premultiplied:
			premultiplied = true;
			pixelType = PixelType('a', 'r', 'g', 'b', 4, 4, 4, 4);
			break;

		case QImage::Format_RGBX8888:
			pixelType = PixelType('r', 'g', 'b', 'x', 8, 8, 8, 8);
			break;

		case QImage::Format_RGBA8888_Premultiplied:
			premultiplied = true;

		case QImage::Format_RGBA8888:
			pixelType = PixelType('r', 'g', 'b', 'a', 8, 8, 8, 8);
			break;

		case QImage::Format_Alpha8:
			pixelType = PixelType('a', 0, 0, 0, 8, 0, 0, 0);
			break;

		case QImage::Format_Grayscale8:
			pixelType = PixelType('i', 0, 0, 0, 8, 0, 0, 0);
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

bool QPVRHandler::write(const QImage &image)
{
	PixelType pixelType;
	bool isPremultiplied = false;
	QImage img = image;
	if (!imageFormatToPvrPixelType(
			image.format(), &pixelType, &isPremultiplied))
	{
		img = image.convertToFormat(QImage::Format_RGB888);
		if (!imageFormatToPvrPixelType(
				img.format(), &pixelType, &isPremultiplied))
			return false;
	}

	mImageFormat = img.format();

	int pixelSize = (img.pixelFormat().bitsPerPixel() + 7) / 8;
	int width = img.width();
	int height = img.height();

	CPVRTextureHeader header(pixelType.PixelTypeID,
							 height,
							 width,
							 1, 1, 1, 1,
							 ePVRTCSpacelRGB,
							 ePVRTVarTypeUnsignedByteNorm,
							 isPremultiplied);

	header.setOrientation((EPVRTOrientation) mOrientation);

	auto widthBytes = img.width() * pixelSize;
	QByteArray data(
		static_cast<int>(
			header.getDataSize(PVRTEX_TOPMIPLEVEL, false, false)), 0);
	auto pvrWidthBytes = data.size() / img.height();
	auto pvrPtr = data.data();

	for (int y = 0; y < height; y++)
	{
		memcpy(pvrPtr, img.scanLine(y), widthBytes);
		pvrPtr += pvrWidthBytes;
	}

	img = QImage();

	CPVRTexture texture(header, data.data());

	bool texV2 = 0 != (mFormat & TEXV2);
	bool tex4bpp = 0 != (mFormat & TEXBIT4);

	PixelType outputPixelType = pixelType;
	ECompressorQuality quality = ePVRTCFastest;
	if (0 != (mFormat & ETC))
	{
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

		switch (int(mQuality * 1))
		{
			case 0:
				quality = eETCFastPerceptual;
				break;

			case 1:
				quality = eETCSlowPerceptual;
				break;

			default:
				break;
		}
	} else
	if (0 != (mFormat & PVRTC))
	{
		if (texV2)
		{
			outputPixelType = tex4bpp
				? ePVRTPF_PVRTCII_4bpp
				: ePVRTPF_PVRTCII_2bpp;
		} else
		{
			if (pvrPixelTypeHasAlpha(outputPixelType.PixelTypeID))
			{
				outputPixelType = tex4bpp
					? ePVRTPF_PVRTCI_4bpp_RGBA
					: ePVRTPF_PVRTCI_2bpp_RGBA;
			} else
			{
				outputPixelType = tex4bpp
					? ePVRTPF_PVRTCI_4bpp_RGB
					: ePVRTPF_PVRTCI_2bpp_RGB;
			}
		}

		quality = ECompressorQuality(int(mQuality * (int(eNumPVRTCModes) - 1)));
	}

	if (pixelType.PixelTypeID == outputPixelType.PixelTypeID ||
		Transcode(
			texture, outputPixelType,
			texture.getChannelType(),
			texture.getColourSpace(),
			quality))
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
				return QVariant::fromValue<QList<QByteArray> >(
					supportedSubTypes());
			}

			case CompressionRatio:
				return mCompressionRatio;

			case Size:
			{
				if (!ensureScanned())
					break;

				return QSize(mTexture->getWidth(), mTexture->getHeight());
			}

			case ScaledSize:
				return mScaledSize;

			case Quality:
				return static_cast<int>(mQuality * 100);

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

		case CompressionRatio:
		{
			int r = value.toInt(&ok);
			if (!ok || r < 0)
				r = Z_DEFAULT_COMPRESSION;

			mCompressionRatio = r;
			break;
		}

		case ScaledSize:
			mScaledSize = value.value<QSize>();
			break;

		case Quality:
		{
			int q = value.toInt(&ok);
			if (!ok || q < 0 || q > 100)
				q = 100;

			mQuality = q / 100.0;
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
		case CompressionRatio:
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
	static const QByteArray s_ccz = QByteArrayLiteral("ccz");
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

	if (s != s_ccz)
	{
		int skip;
		int suffix;
		if (s.startsWith(s_pvrtc))
		{
			result |= PVRTC;
			skip = s_pvrtc.length();
			suffix = 3;
		} else
		if (s.startsWith(s_etc))
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
	}

	if (!split.isEmpty())
	{
		s = split.takeFirst();
	}

	if (s == s_ccz)
	{
		result |= CCZ;
		return result;
	}

	return result;
}

QByteArray QPVRHandler::formatToString(int format)
{
	QByteArray result;

	if (PVR3 == (format & PVR3))
	{
		result += QByteArrayLiteral("pvr3");
	} else
	if (PVR2 == (format & PVR2))
	{
		result += QByteArrayLiteral("pvr2");
	}

	bool compressed = false;
	bool isETC = 0 != (format & ETC);

	if (isETC)
	{
		result += QByteArrayLiteral(".etc");
		compressed = true;
	} else
	if (0 != (format & PVRTC))
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

	if (0 != (format & CCZ))
	{
		result += QByteArrayLiteral(".ccz");
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

	auto device = this->device();
	std::unique_ptr<QCCZDecompressionStream> decompress;
	if (0 != (mFormat & CCZ))
	{
		decompress.reset(new QCCZDecompressionStream(device));
		device = decompress.get();
		if (!device->open(QIODevice::ReadOnly))
		{
			return false;
		}
	}

	Texture tex;
	bool readOk;
	{
		Stream::ptr_type stream(new QIODevicePVRAdapter(device));
		TextureReaderPVR reader(stream);

		readOk = reader.readAsset(tex);
	}	// destroy reader and stream adapter

	if (readOk)
	{
		auto &newHeader = tex.getHeader();

		auto pixelFormat = newHeader.getPixelFormat();
		PixelFormat cvtPixelFormat;

		bool isPremultiplied = newHeader.isPreMultiplied();

		mFormat &= FileFormatMask;

		if (pixelFormat == PixelFormat::RGB_565)
		{
			cvtPixelFormat = pixelFormat;

			mImageFormat = QImage::Format_RGB16;
		} else
		if (pixelFormat == PixelFormat::Intensity8)
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
						mFormat |= TEXV2;

					case CompressedPixelFormat::PVRTCI_2bpp_RGBA:
					case CompressedPixelFormat::PVRTCI_2bpp_RGB:
						mFormat |= PVRTC;
						break;

					case CompressedPixelFormat::PVRTCII_4bpp:
						mFormat |= TEXV2;

					case CompressedPixelFormat::PVRTCI_4bpp_RGBA:
					case CompressedPixelFormat::PVRTCI_4bpp_RGB:
						mFormat |= PVRTC | TEXBIT4;
						break;

					case CompressedPixelFormat::ETC2_RGBA:
					case CompressedPixelFormat::ETC2_RGB_A1:
						mFormat |= TEXV2;

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

		CPVRTextureHeader header(pixelFormat.getPixelTypeId(),
								 newHeader.getHeight(),
								 newHeader.getWidth(),
								 newHeader.getDepth(),
								 newHeader.getNumberOfMIPLevels(),
								 newHeader.getNumberOfArrayMembers(),
								 newHeader.getNumberOfFaces(),
								 (EPVRTColourSpace)newHeader.getColorSpace(),
								 (EPVRTVariableType)newHeader.getChannelType(),
								 isPremultiplied);

		int orientation =
			newHeader.getOrientation(TextureMetaData::AxisAxisX) |
			newHeader.getOrientation(TextureMetaData::AxisAxisY) |
			newHeader.getOrientation(TextureMetaData::AxisAxisZ);

		header.setOrientation((EPVRTOrientation) orientation);

		TexturePtr texture(
			new CPVRTexture(header, tex.getDataPointer()));

		if ((cvtPixelFormat == pixelFormat &&
			 newHeader.getColorSpace() == types::ColorSpace::lRGB &&
			 newHeader.getChannelType() == VariableType::UnsignedByteNorm) ||
			Transcode(
				*texture.data(),
				cvtPixelFormat.getPixelTypeId(),
				ePVRTVarTypeUnsignedByteNorm,
				ePVRTCSpacelRGB,
				(ECompressorQuality) 0))
		{
			mFormat |= PVR3;
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
	std::unique_ptr<AssetWriter<Texture> > writerPtr;

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

	auto device = this->device();
	std::unique_ptr<QCCZCompressionStream> compress;
	if (0 != (mFormat & CCZ))
	{
		compress.reset(
			new QCCZCompressionStream(device, mCompressionRatio));

		device = compress.get();
		if (!device->open(QIODevice::WriteOnly))
		{
			return false;
		}
	}

	bool result;
	{
		Stream::ptr_type stream(new QIODevicePVRAdapter(device));
		result = writer->openAssetStream(stream) && writer->writeAllAssets();
		writerPtr.reset(nullptr);
	}

	return result;
}

bool QPVRHandler::writeTexture(const CPVRTexture &texture)
{
	auto &oldHeader = texture.getHeader();

	TextureHeader header(oldHeader.getPixelType().PixelTypeID,
						 oldHeader.getHeight(),
						 oldHeader.getWidth(),
						 oldHeader.getDepth(),
						 oldHeader.getNumMIPLevels(),
						 (types::ColorSpace)oldHeader.getColourSpace(),
						 (VariableType)oldHeader.getChannelType(),
						 oldHeader.getNumArrayMembers(),
						 oldHeader.getNumFaces());
	header.setOrientation(
		static_cast<TextureMetaData::AxisOrientation>(
			oldHeader.getOrientation(ePVRTAxisX) |
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
		if (mTexture->getWidth(level) == mScaledSize.width() &&
			mTexture->getHeight(level) == mScaledSize.height())
		{
			return level;
		}
	}

	return PVRTEX_TOPMIPLEVEL;
}

quint32 QPVRHandler::getCurrentArrayIndex() const
{
	quint32 curIndex = static_cast<quint32>(mImageCurrentIndex);
	quint32 numFaces = mTexture->getNumFaces();
	return curIndex / numFaces;
}

quint32 QPVRHandler::getCurrentFaceIndex() const
{
	quint32 curIndex = static_cast<quint32>(mImageCurrentIndex);
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
