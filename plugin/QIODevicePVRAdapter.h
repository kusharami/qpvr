#pragma once

#include "PVRCore/Stream.h"

class QIODevice;

class QIODevicePVRAdapter : public pvr::Stream
{
public:
	QIODevicePVRAdapter(QIODevice *adaptee);
	virtual ~QIODevicePVRAdapter() override;

	virtual bool read(
		size_t elementSize, size_t elementCount,
		void *buffer, size_t &elementsRead) const override;

	virtual bool write(
		size_t elementSize, size_t elementCount,
		const void *buffer, size_t &elementsWritten) override;

	virtual bool seek(long offset, SeekOrigin origin) const override;

	virtual bool open() const override;

	virtual void close() override;

	virtual bool isopen() const override;

	virtual size_t getPosition() const override;

	virtual size_t getSize() const override;

private:
	typedef int64_t (QIODevice::*Read)(char *, int64_t);
	typedef int64_t (QIODevice::*Write)(const char *, int64_t);
	using Operation = Read;

	bool ioOperation(
		size_t elementSize, size_t elementCount,
		char *buffer, size_t &resultCount,
		Operation op) const;

	QIODevice *mAdaptee;
	mutable int64_t mOriginalPosition;
	mutable bool mIsOpen;
	mutable bool mTransactionStarted;
};
