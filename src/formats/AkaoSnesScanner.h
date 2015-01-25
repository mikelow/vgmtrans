#pragma once
#include "Scanner.h"
#include "BytePattern.h"

enum AkaoSnesVersion; // see AkaoSnesFormat.h

class AkaoSnesScanner :
	public VGMScanner
{
public:
	AkaoSnesScanner(void)
	{
		USE_EXTENSION(L"spc");
	}
	virtual ~AkaoSnesScanner(void)
	{
	}

	virtual void Scan(RawFile* file, void* info = 0);
	void SearchForAkaoSnesFromARAM(RawFile* file);
	void SearchForAkaoSnesFromROM(RawFile* file);

private:
	static BytePattern ptnReadNoteLengthV1;
	static BytePattern ptnReadNoteLengthV2;
	static BytePattern ptnReadNoteLengthV4;
	static BytePattern ptnVCmdExecV1;
	static BytePattern ptnVCmdExecV4;
	static BytePattern ptnReadSeqHeaderV1;
	static BytePattern ptnReadSeqHeaderV2;
	static BytePattern ptnReadSeqHeaderFFMQ;
	static BytePattern ptnReadSeqHeaderV4;
};
