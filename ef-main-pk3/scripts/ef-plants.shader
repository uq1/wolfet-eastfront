models/mapobjects/plants/bushes3
{
	surfaceparm alphashadow
//	deformVertexes wave 194 sin 0 2 0 .1
//	deformVertexes wave 30 sin 0 1 0 .3
//	deformVertexes wave 194 sin 0 1 0 .4
	cull twosided


	{
		map models/mapobjects/plants/bushes3.tga
		alphaFunc GE128
//		rgbGen lightingdiffuse
		rgbGen identity
	}

}

models/mapobjects/plants/bushes2
{
	surfaceparm alphashadow
//	deformVertexes wave 194 sin 0 2 0 .1
//	deformVertexes wave 30 sin 0 1 0 .3
//	deformVertexes wave 194 sin 0 1 0 .4
	cull twosided


	{
		map models/mapobjects/plants/bushes2.tga
		alphaFunc GE128
		rgbGen lightingdiffuse
//		rgbGen identity
	}

}

models/map_objects/yavin/fern
{
	q3map_nolightmap
	q3map_alphashadow
	q3map_onlyvertexlighting
	cull	twosided
    {
        map models/map_objects/yavin/fern
        alphaFunc GE128
        rgbGen vertex
    }
}

models/map_objects/yavin/plant
{
	q3map_nolightmap
	q3map_alphashadow
	q3map_onlyvertexlighting
	cull	twosided
    {
        map models/map_objects/yavin/plant
        alphaFunc GE128
        rgbGen vertex
    }
}

models/map_objects/yavin/fern3
{
	q3map_nolightmap
	q3map_alphashadow
	q3map_onlyvertexlighting
	cull	twosided
    {
        map models/map_objects/yavin/fern3
        alphaFunc GE128
        rgbGen vertex
    }
    {
        map models/map_objects/yavin/fern3_spec
        blendFunc GL_SRC_ALPHA GL_ONE
        detail
        alphaGen lightingSpecular
    }
}

models/map_objects/yavin/grass
{
	q3map_nolightmap
	q3map_alphashadow
	q3map_onlyvertexlighting
	cull	twosided
    {
        map models/map_objects/yavin/grass.tga
        alphaFunc GE128
        rgbGen vertex
    }
}

models/map_objects/yavin/grass_tall_ORIG
{
	q3map_nolightmap
	q3map_alphashadow
	q3map_onlyvertexlighting
	cull	twosided
    {
        map models/map_objects/yavin/grass_tall.tga
        alphaFunc GE128
        rgbGen vertex
    }
}

models/map_objects/yavin/grass_tall
{
	surfaceparm alphashadow
//	deformVertexes wave 194 sin 0 2 0 .1
//	deformVertexes wave 30 sin 0 1 0 .3
//	deformVertexes wave 194 sin 0 1 0 .4
	cull twosided


	{
		map models/map_objects/yavin/grass_tall.tga
		alphaFunc GE128
		rgbGen lightingdiffuse
//		rgbGen identity
	}

}

models/map_objects/yavin/fern2
{
	q3map_nolightmap
	q3map_alphashadow
	q3map_onlyvertexlighting
	cull	twosided
    {
        map models/map_objects/yavin/fern2
        alphaFunc GE128
        rgbGen vertex
    }
}

models/map_objects/yavin/tree06b
{
	surfaceparm	trans
	q3map_nolightmap
	q3map_alphashadow
	q3map_onlyvertexlighting
	cull	twosided
    {
        map models/map_objects/yavin/tree06b
        alphaFunc GE128
        rgbGen vertex
    }
}

models/map_objects/yavin/tree01btga
{
	q3map_nolightmap
	q3map_alphashadow
	q3map_onlyvertexlighting
	cull	twosided
    {
        map models/map_objects/yavin/tree01btga
        alphaFunc GE128
        rgbGen vertex
    }
}

models/map_objects/yavin/tree05_vines
{
	q3map_nolightmap
	q3map_alphashadow
	q3map_onlyvertexlighting
	cull	twosided
    {
        map models/map_objects/yavin/tree05_vines
        alphaFunc GE128
        rgbGen vertex
    }
}

models/map_objects/yavin/tree09b
{
	q3map_nolightmap
	q3map_alphashadow
	q3map_onlyvertexlighting
	cull	twosided
    {
        map models/map_objects/yavin/tree09b
        alphaFunc GE128
        rgbGen vertex
    }
}

models/map_objects/yavin/tree08b
{
	q3map_nolightmap
	q3map_alphashadow
	q3map_onlyvertexlighting
	cull	twosided
    {
        map models/map_objects/yavin/tree08b
        alphaFunc GE128
        rgbGen vertex
    }
}

models/map_objects/yavin/tree09_vines
{
	q3map_nolightmap
	q3map_alphashadow
	q3map_onlyvertexlighting
	cull	twosided
    {
        map models/map_objects/yavin/tree09_vines
        alphaFunc GE128
        rgbGen vertex
    }
}

models/map_objects/yavin/tree09a
{
	q3map_nolightmap
	q3map_alphashadow
	q3map_onlyvertexlighting
	cull	twosided
    {
        map models/map_objects/yavin/tree09a
        alphaFunc GE128
        rgbGen vertex
    }
}

models/map_objects/yavin/tree09d
{
	q3map_nolightmap
	q3map_alphashadow
	q3map_onlyvertexlighting
	cull	twosided
    {
        map models/map_objects/yavin/tree09d
        alphaFunc GE128
        rgbGen vertex
    }
}

models/map_objects/yavin/tree2
{
	q3map_nolightmap
	q3map_alphashadow
	q3map_onlyvertexlighting
	cull	twosided
    {
        map models/map_objects/yavin/tree2
        alphaFunc GE128
        rgbGen vertex
    }
}

models/map_objects/yavin/vines
{
	q3map_nolightmap
	q3map_alphashadow
	q3map_onlyvertexlighting
	cull	twosided
    {
        map models/map_objects/yavin/vines
        alphaFunc GE128
        rgbGen vertex
    }
}

models/map_objects/yavin/light
{
	q3map_nolightmap
	q3map_onlyvertexlighting
    {
        map models/map_objects/yavin/light
        blendFunc GL_ONE GL_ONE
        glow
        rgbGen lightingDiffuse
    }
    {
        map models/map_objects/yavin/light_glow
        blendFunc GL_ONE GL_ONE
        glow
        tcMod rotate 5
        tcMod scroll 0.5 0
    }
    {
        map models/map_objects/yavin/light_glow2
        blendFunc GL_ONE GL_ONE
        glow
        tcMod scale 3 3
        tcMod scroll 0 0.25
    }
}

models/map_objects/yavin/tree2_b
{
	qer_editorimage	models/map_objects/yavin/tree2
	q3map_nolightmap
	q3map_alphashadow
	q3map_onlyvertexlighting
	cull	twosided
    {
        map models/map_objects/yavin/tree2
        alphaFunc GE128
        rgbGen lightingDiffuse
    }
}

models/map_objects/yavin/tree09d_b
{
	qer_editorimage	models/map_objects/yavin/tree09d
	q3map_alphashadow
	cull	twosided
    {
        map models/map_objects/yavin/tree09d
        alphaFunc GE128
        rgbGen lightingDiffuse
    }
}

models/map_objects/yavin/tree09a_b
{
	qer_editorimage	models/map_objects/yavin/tree09a
	q3map_nolightmap
	q3map_alphashadow
	q3map_onlyvertexlighting
	cull	twosided
    {
        map models/map_objects/yavin/tree09a
        alphaFunc GE128
        rgbGen lightingDiffuse
    }
}

models/map_objects/yavin/tree08b_b
{
	qer_editorimage	models/map_objects/yavin/tree08b
	q3map_nolightmap
	q3map_alphashadow
	q3map_onlyvertexlighting
	cull	twosided
    {
        map models/map_objects/yavin/tree08b
        alphaFunc GE128
        rgbGen lightingDiffuse
    }
}

models/map_objects/yavin/tree09b_b
{
	qer_editorimage	models/map_objects/yavin/tree09b
	q3map_nolightmap
	q3map_alphashadow
	q3map_onlyvertexlighting
	cull	twosided
    {
        map models/map_objects/yavin/tree09b
        alphaFunc GE128
        rgbGen lightingDiffuse
    }
}

models/map_objects/yavin/tree06b_b
{
	qer_editorimage	models/map_objects/yavin/tree06b
	surfaceparm	trans
	q3map_nolightmap
	q3map_onlyvertexlighting
	cull	twosided
    {
        map models/map_objects/yavin/tree06b
        alphaFunc GE128
        rgbGen lightingDiffuse
    }
}

models/map_objects/yavin/grass_tall_b_ORIG
{
	qer_editorimage	models/map_objects/yavin/grass_tall.tga
	q3map_nolightmap
	q3map_alphashadow
	q3map_onlyvertexlighting
	cull	twosided
    {
        map models/map_objects/yavin/grass_tall.tga
        alphaFunc GE128
        rgbGen lightingDiffuse
    }
}

models/map_objects/yavin/grass_tall_b
{
	surfaceparm alphashadow
//	deformVertexes wave 194 sin 0 2 0 .1
//	deformVertexes wave 30 sin 0 1 0 .3
//	deformVertexes wave 194 sin 0 1 0 .4
	cull twosided


	{
		map models/map_objects/yavin/grass_tall.tga
		alphaFunc GE128
		rgbGen lightingdiffuse
//		rgbGen identity
	}

}

models/map_objects/yavin/grass_b
{
	qer_editorimage	models/map_objects/yavin/grass.tga
	q3map_nolightmap
	q3map_alphashadow
	q3map_onlyvertexlighting
	cull	twosided
    {
        map models/map_objects/yavin/grass.tga
        alphaFunc GE128
        rgbGen lightingDiffuse
    }
}

models/map_objects/yavin/fern3_b
{
	qer_editorimage	models/map_objects/yavin/fern3
	q3map_nolightmap
	q3map_alphashadow
	q3map_onlyvertexlighting
	cull	twosided
    {
        map models/map_objects/yavin/fern3
        alphaFunc GE128
        rgbGen lightingDiffuse
    }
}

models/map_objects/yavin/plant_b
{
	qer_editorimage	models/map_objects/yavin/plant
	q3map_nolightmap
	q3map_alphashadow
	q3map_onlyvertexlighting
	cull	twosided
    {
        map models/map_objects/yavin/plant
        alphaFunc GE128
        rgbGen lightingDiffuse
    }
}

models/map_objects/yavin/tree09_vines_b
{
	qer_editorimage	models/map_objects/yavin/tree09_vines
	q3map_nolightmap
	q3map_alphashadow
	q3map_onlyvertexlighting
	cull	twosided
    {
        map models/map_objects/yavin/tree09_vines
        alphaFunc GE128
        rgbGen lightingDiffuse
    }
}

models/map_objects/yavin/vines_b
{
	qer_editorimage	models/map_objects/yavin/vines
	q3map_nolightmap
	q3map_alphashadow
	q3map_onlyvertexlighting
	cull	twosided
    {
        map models/map_objects/yavin/vines
        alphaFunc GE128
        rgbGen lightingDiffuse
    }
}
