#pragma once

#include "PVRCore/Stream.h"

class QIODevice;

class QIODevicePVRAdapter : public pvr::Stream
{
public:
	QIODevicePVRAdapter(QIODevice *adaptee);

	QIODevice *adaptee() const;

	virtual bool read(
		size_t elementSize, size_t elementCount,
		void *buffer, size_t &dataRead) const override;

	virtual bool write(
		size_t elementSize, size_t elementCount,
		const void *buffer, size_t &dataWritten) override;

	virtual bool seek(long offset, SeekOrigin origin) const override;

	virtual bool open() const override;

	virtual void close() override;

	virtual bool isopen() const override;

	virtual size_t getPosition() const override;

	virtual size_t getSize() const override;

private:
	QIODevice *mAdaptee;
	uint64_t mOriginalPosition;
	mutable bool mIsOpen;
};

inline QIODevice *QIODevicePVRAdapter::adaptee() const
{
	return mAdaptee;
}
