// gfx_misc.shader
// generated by ShaderCleaner on Thu Feb  6 12:43:32 2003
// 10 total shaders

gfx/misc/binocsimple
{
	nomipmaps
	nopicmip
	{
		map gfx/misc/binocsimple.tga
		blendfunc filter
	}
}

gfx/misc/head_open
{
	cull twosided
	{
		map gfx/misc/head_open.tga
		alphaFunc GE128
		rgbGen lightingdiffuse
	}
}

gfx/misc/raindrop
{
	cull none
	nomipmaps
	nopicmip
	{
		map gfx/misc/raindrop.tga
		
		//map gfx/atmosphere/raindrop.tga
		//blendfunc GL_ONE GL_ONE
		blendfunc blend
		alphagen vertex
	}
}

gfx/misc/raindropsolid
{
	nopicmip
	polygonoffset
	{
		map gfx/misc/raindropsolid.tga
		blendfunc blend
		alphagen vertex
	}
}

gfx/misc/raindropwater
{
	nopicmip
	cull none
	{
		map gfx/misc/raindropwater.tga
		blendfunc blend
		alphagen vertex
	}
}

gfx/misc/reticle
{
	nomipmaps
	nopicmip
	{
		map gfx/misc/reticle_eq.tga
		blendfunc filter
	}
}

gfx/misc/reticlesimple
{
	nomipmaps
	nopicmip
	{
		map gfx/misc/reticle_eq.tga
		blendfunc filter
	}
}

gfx/misc/snooper
{
	nomipmaps
	nopicmip
	{
		map gfx/misc/reticle_eq.tga
		blendfunc GL_ONE_MINUS_DST_COLOR GL_ONE_MINUS_SRC_COLOR
	}
}

gfx/misc/snow
{
	cull none
	entitymergable
	nofog
	//nomipmaps
	nopicmip
	sort decal
	{
		clampmap gfx/misc/snowflake.tga
		blendfunc blend
		//alphafunc GE128
		rgbgen vertex
	}
}

gfx/misc/speaker
{
	cull none
	nofog
	nomipmaps
	nopicmip
	{
		map gfx/misc/speaker.tga
		alphaFunc GE128
		rgbgen vertex
	}
}

gfx/misc/speaker_gs
{
	cull none
	nofog
	nomipmaps
	nopicmip
	{
		map gfx/misc/speaker_gs.tga
		alphaFunc GE128
		rgbgen vertex
	}
}

gfx/misc/tracer
{
	nopicmip
	cull none
	{
		map gfx/misc/tracer2.tga
		blendFunc GL_ONE GL_ONE
	}
}

gfx/misc/adv_scope_trans
{
	nopicmip
	{
		map gfx/misc/adv_scope_trans.tga
		blendFunc filter
		rgbGen identity
	}
	{
		map gfx/misc/adv_scope_blend.tga
		blendFunc GL_ONE GL_ONE
		rgbGen identity
	}
}

gfx/misc/adv_scope
{
	nopicmip
	{
		map gfx/misc/adv_scope_alpha.tga
		rgbGen identity
		blendFunc blend
	}
//	{
//		map gfx/misc/adv_scope_trans.tga
//		blendFunc filter
//		rgbGen identity
//	}
	{
		map gfx/misc/adv_scope_blend.tga
		//blendFunc GL_ONE GL_ONE
		blendfunc GL_ONE_MINUS_DST_COLOR GL_ONE_MINUS_SRC_COLOR
		rgbGen identity
	}
}

