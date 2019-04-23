#include "PVRTextureWrapper.h"

#ifdef __MINGW32__

#include <_mingw.h>
#include <windows.h>
#include <memory>
#include <mutex>

struct PVRTexLib
{
	HMODULE handle;

	FARPROC construct_header;
	FARPROC destruct_header;
	FARPROC construct_texture;
	FARPROC destruct_texture;
	FARPROC transcode_p;
	FARPROC get_data_pointer;

	PVRTexLib();
	~PVRTexLib();

	void constructTexture(void *data, const pvr::Texture &tex);
	void destructTexture(void *data);
	bool transcode(
		void *data, const pvr::PixelFormat &ptFormat, uint32_t eQuality);

	const void *getDataPointer(void *data);
};

#ifdef __MINGW64_VERSION_MAJOR

static const char sConstructHeader[] = "??0CPVRTextureHeader@pvrtexture@@"
									   "QEAA@_KIIIIIIW4EPVRTColourSpace@@"
									   "W4EPVRTVariableType@@_N@Z";

static const char sDestructHeader[] = "??1CPVRTextureHeader@"
									  "pvrtexture@@QEAA@XZ";

static const char sConstructTexture[] = "??0CPVRTexture@pvrtexture@@"
										"QEAA@AEBVCPVRTextureHeader@1@PEBX@Z";

static const char sDestructTexture[] = "??1CPVRTexture@pvrtexture@@QEAA@XZ";

static const char sTranscode[] = "?Transcode@pvrtexture@@"
								 "YA_NAEAVCPVRTexture@1@TPixelType"
								 "@1@W4EPVRTVariableType@@"
								 "W4EPVRTColourSpace@@"
								 "W4ECompressorQuality@1@_N@Z";

static const char sGetDataPointer[] = "?getDataPtr@CPVRTexture@"
									  "pvrtexture@@QEBAPEAXIII@Z";

#else

static const char sConstructHeader[] = "??0CPVRTextureHeader@pvrtexture@@"
									   "QAE@_KIIIIIIW4EPVRTColourSpace@@"
									   "W4EPVRTVariableType@@_N@Z";

static const char sDestructHeader[] = "??1CPVRTextureHeader@pvrtexture@@QAE@XZ";

static const char sConstructTexture[] = "??0CPVRTexture@pvrtexture@@"
										"QAE@ABVCPVRTextureHeader@1@PBX@Z";

static const char sDestructTexture[] = "??1CPVRTexture@pvrtexture@@QAE@XZ";

static const char sTranscode[] = "?Transcode@pvrtexture@@YA_NAAVCPVRTexture"
								 "@1@TPixelType@1@W4EPVRTVariableType@@"
								 "W4EPVRTColourSpace@@"
								 "W4ECompressorQuality@1@_N@Z";

static const char sGetDataPointer[] = "?getDataPtr@CPVRTexture@"
									  "pvrtexture@@QBEPAXIII@Z";

#endif

PVRTexLib::PVRTexLib()
{
	handle = LoadLibraryA("PVRTexLib.dll");
	if (handle)
	{
		construct_header = GetProcAddress(handle, sConstructHeader);
		destruct_header = GetProcAddress(handle, sDestructHeader);
		construct_texture = GetProcAddress(handle, sConstructTexture);
		destruct_texture = GetProcAddress(handle, sDestructTexture);
		transcode_p = GetProcAddress(handle, sTranscode);
		get_data_pointer = GetProcAddress(handle, sGetDataPointer);
	}
}

PVRTexLib::~PVRTexLib()
{
	if (handle)
		FreeLibrary(handle);
}

void PVRTexLib::constructTexture(void *data, const pvr::Texture &tex)
{
	uint8_t textureHeader[112];

	uint64_t pixelTypeId = tex.getPixelFormat().getPixelTypeId();
	uint32_t height = tex.getHeight();
	uint32_t width = tex.getWidth();
	uint32_t depth = tex.getDepth();
	uint32_t numMipMaps = tex.getNumberOfMIPLevels();
	uint32_t numArrays = tex.getNumberOfArrayMembers();
	uint32_t numFaces = tex.getNumberOfFaces();
	uint32_t colorSpace = uint32_t(tex.getColorSpace());
	uint32_t channelType = uint32_t(tex.getChannelType());
	uint32_t premultiplied = tex.isPreMultiplied();
	auto dataPointer = tex.getDataPointer();

#ifdef __MINGW64_VERSION_MAJOR
	asm(
		/// call CPVRTextureHeader constructor
		"movl %10,%%ecx;"
		"pushq %%rcx;"
		"movl %9,%%ecx;"
		"pushq %%rcx;"
		"movl %8,%%ecx;"
		"pushq %%rcx;"
		"movl %7,%%ecx;"
		"pushq %%rcx;"
		"movl %6,%%ecx;"
		"pushq %%rcx;"
		"movl %5,%%ecx;"
		"pushq %%rcx;"
		"movl %4,%%ecx;"
		"pushq %%rcx;"
		"movl %3,%%r9d;"
		"movl %2,%%r8d;"
		"movq %1,%%rdx;"
		"lea %0,%%rcx;"
		"movq %11,%%rax;"
		"subq $32,%%rsp;"
		"call *%%rax;"
		/// call CPVRTexture constructor
		"addq $56,%%rsp;"
		"movq %14,%%r8;"
		"lea %0,%%rdx;"
		"movq %13,%%rcx;"
		"movq %12,%%rax;"
		"call *%%rax;"
		/// call CPVRTextureHeader destructor
		"lea %0,%%rcx;"
		"movq %15,%%rax;"
		"call *%%rax;"
		"addq $32,%%rsp;"
		:
		: "m"(textureHeader), "m"(pixelTypeId), "m"(height), "m"(width),
		"m"(depth), "m"(numArrays), "m"(numFaces), "m"(numMipMaps),
		"m"(colorSpace), "m"(channelType), "m"(premultiplied),
		"m"(construct_header), "m"(construct_texture), "m"(data),
		"m"(dataPointer), "m"(destruct_header)
		: "rax", "rcx", "rdx", "r8", "r9", "r10", "r11");
#else
	// TODO
#endif
}

void PVRTexLib::destructTexture(void *data)
{
#ifdef __MINGW64_VERSION_MAJOR
	asm("subq $32,%%rsp;"
		/// call CPVRTexture destructor
		"call *%%rax;"
		"addq $32,%%rsp;"
		:
		: "c"(data), "a"(destruct_texture)
		: "rdx", "r8", "r9", "r10", "r11");
#else
//TODO
#endif
}

bool PVRTexLib::transcode(
	void *data, const pvr::PixelFormat &ptFormat, uint32_t eQuality)
{
	uint64_t pixelTypeId = ptFormat.getPixelTypeId();
	bool result;
#ifdef __MINGW64_VERSION_MAJOR
	asm(
		/// call Transcode
		"xorq %%r8,%%r8;"
		"pushq %%r8;" // dither
		"movl %4,%%r8d;"
		"pushq %%r8;" // quality
		"xorq %%r9,%%r9;" // lRGB
		"xorq %%r8,%%r8;" // UnsignedByteNorm
		"movq %3,%%rdx;"
		"movq %2,%%rcx;"
		"movq %1,%%rax;"
		"subq $32,%%rsp;"
		"call *%%rax;"
		"addq $48,%%rsp;"
		: "=a"(result)
		: "m"(transcode_p), "m"(data), "m"(pixelTypeId), "m"(eQuality)
		: "rcx", "rdx", "r8", "r9", "r10", "r11");

#else
	//TODO
#endif
	return result;
}

const void *PVRTexLib::getDataPointer(void *data)
{
	const void *result;
#ifdef __MINGW64_VERSION_MAJOR
	asm("subq $32,%%rsp;"
		"xor %%r9,%%r9;"
		"xor %%r8,%%r8;"
		"xor %%rdx,%%rdx;"
		"call *%%rax;"
		"addq $32,%%rsp"
		: "=a"(result)
		: "a"(get_data_pointer), "c"(data)
		: "rdx", "r8", "r9");
#else
	// TODO
#endif
	return result;
}

static std::mutex _pvrTexLibMutex;
static std::unique_ptr<PVRTexLib> _pvrTexLib;

static PVRTexLib *getPVRTexLib()
{
	std::lock_guard<std::mutex> lock(_pvrTexLibMutex);
	if (!_pvrTexLib)
		_pvrTexLib.reset(new PVRTexLib);
	return _pvrTexLib.get();
}

#else

#include "PVRTextureDefines.h"
#include "PVRTextureFormat.h"
#include "PVRTString.h"
#include "PVRTMap.h"
#include "PVRTexture.h"
#include "PVRTextureUtilities.h"

#endif

PVRTextureWrapper::PVRTextureWrapper(const pvr::PixelFormat &ptFormat,
	uint32_t width, uint32_t height, int orientation, bool premultiplied)
	: pvr::Texture(pvr::TextureHeader(ptFormat, width, height, 1, 1,
		  pvr::types::ColorSpace::lRGB, pvr::VariableType::UnsignedByteNorm, 1,
		  1, premultiplied ? Header::PremultipliedFlag : 0))
{
	setOrientation(pvr::TextureMetaData::AxisOrientation(orientation));
}

PVRTextureWrapper::PVRTextureWrapper(const pvr::Texture &other)
	: pvr::Texture(other)
{
}

PVRTextureWrapper::PVRTextureWrapper(
	const pvr::TextureHeader &sHeader, const void *pData)
	: pvr::Texture(sHeader, reinterpret_cast<const uint8_t *>(pData))
{
}

void PVRTextureWrapper::setTextureData(
	const pvr::PixelFormat &ptFormat, const void *data)
{
	TextureHeader header(ptFormat, getWidth(), getHeight(), getDepth(),
		getNumberOfMIPLevels(), pvr::types::ColorSpace::lRGB,
		pvr::VariableType::UnsignedByteNorm, getNumberOfArrayMembers(),
		getNumberOfFaces(), isPreMultiplied() ? Header::PremultipliedFlag : 0);
	decltype(_metaDataMap) backupMetaData;
	backupMetaData.swap(_metaDataMap);
	uint32_t metaDataSize = _header.metaDataSize;
	initializeWithHeader(header);
	backupMetaData.swap(_metaDataMap);
	_header.metaDataSize = metaDataSize;
	memcpy(getDataPointer(), data, getDataSize());
}

bool PVRTextureWrapper::transcode(
	const pvr::PixelFormat &ptFormat, int eQuality)
{
	if (ptFormat == getPixelFormat() &&
		getColorSpace() == pvr::types::ColorSpace::lRGB &&
		getChannelType() == pvr::VariableType::UnsignedByteNorm)
	{
		return true;
	}

#ifdef __MINGW32__
	auto texLib = getPVRTexLib();
	if (!texLib || !texLib->handle)
		return false;

	uint8_t internalData[128];
	texLib->constructTexture(internalData, *this);

	bool ok = texLib->transcode(internalData, ptFormat, eQuality);

	if (ok)
	{
		setTextureData(ptFormat, texLib->getDataPointer(internalData));
	}

	texLib->destructTexture(internalData);
	return ok;
#else
	using namespace pvrtexture;

	CPVRTextureHeader header(getPixelFormat().getPixelTypeId(), getHeight(),
		getWidth(), getDepth(), getNumberOfMIPLevels(),
		getNumberOfArrayMembers(), getNumberOfFaces(),
		EPVRTColourSpace(getColorSpace()), EPVRTVariableType(getChannelType()),
		isPreMultiplied());

	CPVRTexture texture(header, getDataPointer());
	if (Transcode(texture, ptFormat.getPixelTypeId(),
			ePVRTVarTypeUnsignedByteNorm, ePVRTCSpacelRGB,
			pvrtexture::ECompressorQuality(eQuality), false))
	{
		setTextureData(ptFormat, texture.getDataPtr());
		return true;
	}

	return false;
#endif
}
