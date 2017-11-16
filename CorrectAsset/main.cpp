#include "CorrectAsset.h"


int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		fprintf(stderr, "Error args! Please input filePath(eg: ./DBConfig.cfg ./invalidAsset.csv\n");
		return -1;
	}

	std::string configStr = argv[1];
	std::string invalidAssetStr = argv[2];

	CorrectAsset correstAsset;
	if (!correstAsset.Init(configStr))
	{
		return -1;
	}

	if (!correstAsset.LoadFile(invalidAssetStr, FT_INVALID_ASSET))
	{
		return -1;
	}

	correstAsset.OnCorrcetAsset();

	return 0;
}