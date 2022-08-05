textures/mp_railgun/fog
{
	qer_editorimage textures/sfx/fog_grey1.tga
	q3map_globaltexture
	q3map_surfacelight 0
	fogparms ( 1 1 1 ) 3128
	surfaceparm fog
	surfaceparm nomarks
	surfaceparm nonsolid
	surfaceparm pointlight
	surfaceparm trans
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc gl_dst_color gl_zero
		rgbgen identity
		tcmod scale -.05 -.05
		tcmod scroll .01 -.01
	}
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc gl_dst_color gl_zero
		rgbgen identity
		tcmod scale .05 .05
		tcmod scroll .01 -.01
	}
}

// ocean fog water
textures/mp_seawall/fog_water
{
	qer_editorimage textures/common/fog_water.tga
	surfaceparm nodraw
  	surfaceparm nonsolid
  	surfaceparm trans
  	surfaceparm water
}

textures/mp_seawall/water_fog
{
	qer_editorimage textures/common/fog_water.tga
	surfaceparm nodraw
  	surfaceparm nonsolid
  	surfaceparm trans
  	surfaceparm fog
  	
  	fogparms ( 0.3137 0.36 0.4039 ) 256
}

textures/mp_wurzburg/fog
{
	qer_editorimage textures/sfx/fog_grey1.tga
	
	surfaceparm nodraw
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm fog
	
	fogparms ( 1 1 1 ) 3192
}

textures/radar/fog
{
	qer_editorimage textures/sfx/fog_grey1.tga
	
	surfaceparm nodraw
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm fog
	
	fogparms ( 1 1 1 ) 3192
}

textures/sfx/fog_water
{
	qer_editorimage textures/sfx/fog_grey1.tga
	q3map_globaltexture
	q3map_surfacelight 0
	fogparms ( .05 .10 .13 ) 128
	surfaceparm fog
	surfaceparm nomarks
	surfaceparm nonsolid
	surfaceparm pointlight
	surfaceparm trans
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc gl_dst_color gl_zero
		rgbgen identity
		tcmod scale -.05 -.05
		tcmod scroll .01 -.01
	}
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc gl_dst_color gl_zero
		rgbgen identity
		tcmod scale .05 .05
		tcmod scroll .01 -.01
	}
}

