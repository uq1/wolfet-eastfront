models/mapobjects/tanks_sd/jagdpanther_full_snow
{
	qer_editorimage models/mapobjects/tanks_sd/jagdpanther_full_snow.tga
	{
		map models/mapobjects/tanks_sd/jagdpanther_snow.tga
		rgbGen lightingDiffuse
	}
}

models/mapobjects/tanks_sd/churchill_flat_temperate
{
	qer_editorimage models/mapobjects/tanks_sd/churchill_flat_temperate.tga
	{
		map models/mapobjects/tanks_sd/churchill_flat_temperate.tga
		rgbGen lightingDiffuse
	}
}


models/mapobjects/tanks_sd/bits_forward_temperate
{
	qer_alphafunc gequal 0.5
	cull disable
	{
		map models/mapobjects/tanks_sd/tracks_temperate.jpg
		alphaFunc GE128
		rgbGen lightingDiffuse
		tcMod scroll 0 2
	}
}


models/mapobjects/tanks_sd/bits_static_temperate
{
	qer_alphafunc gequal 0.5
	cull disable
	{
		map models/mapobjects/tanks_sd/tracks_temperate.jpg
		alphaFunc GE128
		rgbGen lightingDiffuse
	}
}
