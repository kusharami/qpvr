#pragma once

#include <QImageIOHandler>

class QPVRHandler : public QImageIOHandler
{
public:
	QPVRHandler();

	enum Format
	{
		UnknownFormat = 0,
		PVR1 = (1 << 0),
		PVR2 = (1 << 1),
		PVR3 = (1 << 2),
		CCZ = (1 << 3),
		PVR1_CCZ = PVR1 | CCZ,
		PVR2_CCZ = PVR2 | CCZ,
		PVR3_CCZ = PVR3 | CCZ
	};

	static Format detectFormat(QIODevice *device);

	virtual bool canRead() const override;
	virtual bool read(QImage *image) override;
	virtual bool write(const QImage &image) override;

	virtual QVariant option(ImageOption option) const override;
	virtual void setOption(ImageOption option, const QVariant &value) override;
	virtual bool supportsOption(ImageOption option) const override;

	virtual bool jumpToImage(int imageNumber) override;
	virtual int imageCount() const override;

private:
	mutable Format mFormat;
};
