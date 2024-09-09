#include "Texture.h"
#include "IArchive.h"

void TextureData::Serialize(IArchive* ar)
{
#define ASTORE(val) ar->PushObj(#val); *ar << val; ar->PopObj();
#define ALOAD(val) ar->PushObj(#val); *ar >> val; ar->PopObj();

#define ASTOREN(val, name) ar->PushObj(name); *ar << val; ar->PopObj();
#define ALOADN(val, name) ar->PushObj(name); *ar >> val; ar->PopObj();

	int version = 1;
	if (ar->IsStoring())
	{
		ar->PushObj("TextureData");
			*ar << version;
			ASTORE(Path)
			ASTORE(Name)
			ASTORE(WidthPx)
			ASTORE(HeightPx)
			ASTOREN((u32)Format, "Format")
			ASTOREN((u32)RowAlignment, "RowAlignment")
			ASTOREN((u32)MinifyFiltering, "MinifyFiltering")
			ASTOREN((u32)MagnifyFiltering, "MagnifyFiltering")
			ASTOREN((u32)TextureClampS, "TextureClampS")
			ASTOREN((u32)TextureClampT, "TextureClampT")
			ASTORE(borderColour)
			ASTORE(bGenerateMipMaps)
			ASTORE(DataSize)

		ar->PopObj();
	}
	else
	{
		ar->PushObj("TextureData");
			int version;
			*ar >> version;
			switch (version)
			{
			case 1:
				ALOAD(Path)
				ALOAD(Name)
				ALOAD(WidthPx)
				ALOAD(HeightPx)
				ALOADN((u32&)Format, "Format")
				ALOADN((u32&)RowAlignment, "RowAlignment")
				ALOADN((u32&)MinifyFiltering, "MinifyFiltering")
				ALOADN((u32&)MagnifyFiltering, "MagnifyFiltering")
				ALOADN((u32&)TextureClampS, "TextureClampS")
				ALOADN((u32&)TextureClampT, "TextureClampT")
				ALOAD(borderColour)
				ALOAD(bGenerateMipMaps)
				ALOAD(DataSize)
				break;
			default:
				break;
			}
		ar->PopObj();
	}

#undef ALOAD
#undef ASTORE
#undef ALOADN
#undef ASTOREN

}
