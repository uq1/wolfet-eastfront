// ydnar: haloed style 
playershadow2
{ 
     cull none 
     deformVertexes wave 1 sin -0.5 0 0 1
     noPicmip 
     surfaceparm trans 
     { 
          map gfx/2d/playershadow.tga 
          blendFunc GL_SRC_ALPHA GL_ONE 
          rgbGen const ( 0.0 0.0 0.0 )
          tcGen environment 
          tcMod scroll 0.025 -0.07625 
     } 
}

playershadow3
{
	//polygonOffset
	surfaceparm nomarks
	{
		map gfx/2d/playershadow.tga 
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		//rgbGen wave noise 0.2 0.8 0.5 13
	}
}

playershadow4
{
	qer_alphafunc gequal 0.5
	cull disable
	{
		clampmap gfx/2d/playershadow.tga 
		alphaFunc GE128
		rgbGen lightingDiffuse
	}
}

playershadow_orig
{
	nopicmip
    {
	map gfx/2d/playershadow.tga 
    }
}

playershadow
{
	polygonOffset
	{
		clampmap gfx/2d/playershadow.tga
		blendFunc GL_ZERO GL_ONE_MINUS_SRC_COLOR
		rgbGen exactVertex
	}
}


