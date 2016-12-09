#pragma once

#include <QImageIOHandler>

class QPVRHandler : public QImageIOHandler
{
public:
	QPVRHandler();

	virtual QByteArray name() const;

	virtual bool canRead() const override;
	virtual bool read(QImage *image) override;
	virtual bool write(const QImage &image) override;

	virtual QVariant option(ImageOption option) const override;
	virtual void setOption(ImageOption option, const QVariant &value) override;
	virtual bool supportsOption(ImageOption option) const override;

	virtual bool jumpToImage(int imageNumber) override;
	virtual int imageCount() const override;
};

