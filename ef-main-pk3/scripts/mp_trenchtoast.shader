textures/skies/sky_forest
{
	qer_editorimage textures/skies/topclouds.tga
	q3map_lightimage textures/skies/n_blue.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nodlight
	q3map_globaltexture
	q3map_lightsubdivide 512
	q3map_sun	1.0 0.8 0.6 130 220 50
	q3map_surfacelight 25							

	fogvars ( .3 .3 .305 ) .000095
	skyfogvars ( .23 .23 .24 ) .1

	skyparms full 200 -
	
	{
		map textures/skies/newclouds.tga
		tcMod scroll -0.001 -0.003
		tcMod scale 6 6
		depthWrite
	}
}

textures/skies/sky_forest_block
{
	qer_editorimage textures/skies/topclouds.tga
	q3map_lightimage textures/skies/n_blue.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nodlight
	q3map_globaltexture
	q3map_lightsubdivide 512

			
	skyparms full 200 -
	
	{
		map textures/skies/newclouds.tga
		tcMod scroll -0.001 -0.003
		tcMod scale 6 6
		depthWrite
	}
}

///////////////// forest ////////////////////////
/////////////////////////////////////////////////

textures/terrain/mxterra3_0
{
	surfaceparm snowsteps
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight	
	{
		map textures/stone/mxrock0b.tga
		rgbGen vertex
		tcmod scale 0.1 0.1
	}
	{
		map textures/skies2/clouds2.tga
		blendfunc filter
		tcmod scale 0.01 0.01
		tcMod scroll -0.01 -0.05
	}
}

textures/terrain/mxterra3_1
{
	surfaceparm snowsteps
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight	
	{
		map textures/stone/mxrock1b.tga
		rgbGen vertex
		tcmod scale 0.1 0.1 
	}
	{
		map textures/skies2/clouds2.tga
		blendfunc filter
		tcmod scale 0.01 0.01
		tcMod scroll -0.01 -0.05
	}
}

textures/terrain/mxterra3_2
{
	surfaceparm snowsteps
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight	
	{
		map textures/stone/mxrock2b.tga
		tcmod scale 0.05 0.05 
		rgbGen vertex
	}
	{
		map textures/skies2/clouds2.tga
		blendfunc filter
		tcmod scale 0.01 0.01
		tcMod scroll -0.01 -0.05
	}
}

textures/terrain/mxterra3_3
{
  surfaceparm snowsteps
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight	
	{
		map textures/stone/mxrock3g.tga
		tcmod scale 0.05 0.05 
		rgbGen vertex
	}
	{
		map textures/skies2/clouds2.tga
		blendfunc filter
		tcmod scale 0.01 0.01
		tcMod scroll -0.01 -0.05
	}
}

textures/terrain/mxterra3_4
{
  surfaceparm snowsteps
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight	
	{
		map textures/stone/mxrock4b.tga
		tcmod scale 0.1 0.1 
		rgbGen vertex
	}
	{
		map textures/skies2/clouds2.tga
		blendfunc filter
		tcmod scale 0.01 0.01
		tcMod scroll -0.01 -0.05
	}
}

textures/terrain/mxterra3_0to1
{
	surfaceparm snowsteps
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight	
	{
		map textures/stone/mxrock0b.tga	
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.1 0.1 
	}
	{
		map textures/stone/mxrock1b.tga
		tcmod scale 0.1 0.1
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/skies2/clouds2.tga
		blendfunc filter
		tcmod scale 0.01 0.01
		tcMod scroll -0.01 -0.05
	}
}

textures/terrain/mxterra3_0to2
{
	surfaceparm snowsteps
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight	
	{
		map textures/stone/mxrock0b.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.1 0.1
	}
	{
		map textures/stone/mxrock2b.tga
		tcmod scale 0.05 0.05 
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/skies2/clouds2.tga
		blendfunc filter
		tcmod scale 0.01 0.01
		tcMod scroll -0.01 -0.05
	}
}

textures/terrain/mxterra3_0to3
{
	surfaceparm snowsteps
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight	
	{
		map textures/stone/mxrock0b.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.1 0.1
	}
	{
		map textures/stone/mxrock3g.tga
		tcmod scale 0.05 0.05 
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/skies2/clouds2.tga
		blendfunc filter
		tcmod scale 0.01 0.01
		tcMod scroll -0.01 -0.05
	}
}

textures/terrain/mxterra3_0to4
{
	surfaceparm snowsteps
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight	
	{
		map textures/stone/mxrock0b.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.1 0.1
	}
	{
		map textures/stone/mxrock4b.tga
		tcmod scale 0.1 0.1 
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/skies2/clouds2.tga
		blendfunc filter
		tcmod scale 0.01 0.01
		tcMod scroll -0.01 -0.05
	}
}


textures/terrain/mxterra3_1to2
{
	surfaceparm snowsteps
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight	
	{
		map textures/stone/mxrock1b.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.1 0.1 
	}
	{
		map textures/stone/mxrock2b.tga
		tcmod scale 0.05 0.05 
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/skies2/clouds2.tga
		blendfunc filter
		tcmod scale 0.01 0.01
		tcMod scroll -0.01 -0.05
	}
}

textures/terrain/mxterra3_1to3
{
	surfaceparm snowsteps
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight	
	{
		map textures/stone/mxrock1b.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.1 0.1 
	}
	{
		map textures/stone/mxrock3g.tga
		tcmod scale 0.05 0.05 
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/skies2/clouds2.tga
		blendfunc filter
		tcmod scale 0.01 0.01
		tcMod scroll -0.01 -0.05
	}
}

textures/terrain/mxterra3_1to4
{
	surfaceparm snowsteps
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight	
	{
		map textures/stone/mxrock1b.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.1 0.1 
	}
	{
		map textures/stone/mxrock4b.tga
		tcmod scale 0.1 0.1 
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/skies2/clouds2.tga
		blendfunc filter
		tcmod scale 0.01 0.01
		tcMod scroll -0.01 -0.05
	}
}

textures/terrain/mxterra3_2to3
{
	surfaceparm snowsteps
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight	
	{
		map textures/stone/mxrock2b.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.05 0.05 
	}
	{
		map textures/stone/mxrock3g.tga
		tcmod scale 0.05 0.05 
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/skies2/clouds2.tga
		blendfunc filter
		tcmod scale 0.01 0.01
		tcMod scroll -0.01 -0.05
	}
}

textures/terrain/mxterra3_2to4
{
	surfaceparm snowsteps
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight	
	{
		map textures/stone/mxrock2b.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.05 0.05 
	}
	{
		map textures/stone/mxrock4b.tga
		tcmod scale 0.1 0.1 
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/skies2/clouds2.tga
		blendfunc filter
		tcmod scale 0.01 0.01
		tcMod scroll -0.01 -0.05
	}
}

textures/terrain/mxterra3_3to4
{
  surfaceparm snowsteps
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight	
	{
		map textures/stone/mxrock3g.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.05 0.05 
	}
	{
		map textures/stone/mxrock4b.tga
		tcmod scale 0.1 0.1 
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/skies2/clouds2.tga
		blendfunc filter
		tcmod scale 0.01 0.01
		tcMod scroll -0.01 -0.05
	}
}

textures/xlab_wall/xmetal_m02f
{
	surfaceparm metalsteps
	qer_editorimage textures/xlab_wall/xmetal_m02.tga
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/xlab_wall/xmetal_m02.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

// Added by Brandon

textures/skies/km_beachsky
{
	qer_editorimage textures/skies/beachsky_m01
	q3map_lightimage textures/skies/n_blue2.tga
	nocompress
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nodlight
	q3map_globaltexture
	q3map_lightsubdivide 2048
	q3map_sun	0.274632 0.274632 0.39 35 120 45
	q3map_surfacelight 30

	skyparms full 200 -
	
	{
		map textures/skies/newclouds2.tga
	//	tcMod scroll 0.005 0.0005
		tcMod scale 16.0 16.0
		depthWrite
	}	

}

// Added by Brandon

textures/skies/sky_baseout2
{
	nocompress
	qer_editorimage textures/skies/beachsky_m01
	q3map_lightimage textures/skies/n_blue2.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
	q3map_globaltexture
	q3map_lightsubdivide 512 
	q3map_sun	0.274632 0.274632 0.39 30 35 35
	q3map_surfacelight 50
	skyparms full 200 -
	fogvars ( .1 .1 .1 ) .0002
	skyfogvars ( .23 .23 .24 ) .05

	
	{
		map textures/skies/newclouds2.tga
//		tcMod scroll 0.0005 0.0001
		tcMod scale 16.0 16.0
		depthWrite
	}

	{
		map textures/skies/vil_clouds1.tga
		blendfunc blend
		tcMod scroll 0.001 0.00
		tcMod scale 2 1
	}
}

textures/assault_rock/ground_c07nerve
{
	surfaceparm snowsteps
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/assault_rock/ground_c07a.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/assault_rock/ground_c09nerve
{
	surfaceparm snowsteps
	{
		map $lightmap
		rgbGen identity		
	}
	{
		map textures/assault_rock/ground_c09a.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity		
	}
}

// stone/mxrock0b >> stone/mxrock0 >> assault_rock/ground_c07a
// stone/mxrock2b >> assault_rock/ground_c07a
// stone/mxrock1b >> terrain/grass_ml03cmp
// stone/mxrock4b >> assault_rock/ground_c09a

textures/sfx/saftey_glass
{
	qer_editorimage textures/common/s_glass.tga		
	surfaceparm alphashadow
	surfaceparm glass 
	surfaceparm trans
//	cull none
	qer_trans 	0.5
	surfaceparm trans

	{
		map textures/common/s_glass.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}

}

// LAST MODIFIED BY MBJ

textures/skies/rf_wrecked_castlesky_trench
{
	qer_editorimage textures/skies/topclouds.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	q3map_globaltexture
	q3map_lightsubdivide 512 
	q3map_surfacelight 85
	fogvars ( .3 .3 .3 ) 3200
	skyfogvars ( .23 .23 .24 ) .005

	surfaceparm nodlight	// (SA) added so dlights don't affect surface

	skyparms full 200 -
	
	{
		map textures/skies/dimclouds.tga
		tcMod scroll 0.002 0.002
		tcMod scale 1 1
		tcMod turb 0 0.05 0 .08

		depthWrite
	}
	{
		map textures/skies/dimclouds.tga
		blendfunc GL_ONE GL_ONE
		tcMod scroll -0.001 -0.005
		tcMod scale 10 10
	}
}

///////////////// Trenchtoast ////////////////
//////////////////////////////////////////////

textures/nerve-guf/trenchterra1_0
{
	surfaceparm snowsteps
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight	
	{
		map textures/assault_rock/ground_c07a.jpg
		rgbGen vertex
		tcmod scale 0.1 0.1
	}
}

textures/nerve-guf/trenchterra1_1
{
	surfaceparm snowsteps
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight	
	{
		map textures/terrain/grass_ml03cmp.jpg
		rgbGen vertex
		tcmod scale 0.1 0.1 
	}	
}

textures/nerve-guf/trenchterra1_2
{
	surfaceparm snowsteps
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight	
	{
		map textures/assault_rock/ground_c07a.jpg
		tcmod scale 0.05 0.05 
		rgbGen vertex
	}
}

textures/nerve-guf/trenchterra1_3
{
  surfaceparm snowsteps
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight	
	{
		map textures/terrain/mountain_m09.jpg
		tcmod scale 0.05 0.05 
		rgbGen vertex
	}
}

textures/nerve-guf/trenchterra1_4
{
  surfaceparm snowsteps
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight	
	{
		map textures/assault_rock/ground_c09a.jpg
		tcmod scale 0.1 0.1 
		rgbGen vertex
	}
}

textures/nerve-guf/trenchterra1_0to1
{
	surfaceparm snowsteps
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight	
	{
		map textures/assault_rock/ground_c07a.jpg	
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.1 0.1 
	}
	{
		map textures/terrain/grass_ml03cmp.jpg
		tcmod scale 0.1 0.1
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

textures/nerve-guf/trenchterra1_0to2
{
	surfaceparm snowsteps
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight	
	{
		map textures/assault_rock/ground_c07a.jpg
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.1 0.1
	}
	{
		map textures/assault_rock/ground_c07a.jpg
		tcmod scale 0.05 0.05 
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

textures/nerve-guf/trenchterra1_0to3
{
	surfaceparm snowsteps
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight	
	{
		map textures/assault_rock/ground_c07a.jpg
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.1 0.1
	}
	{
		map textures/terrain/mountain_m09.jpg
		tcmod scale 0.05 0.05 
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

textures/nerve-guf/trenchterra1_0to4
{
	surfaceparm snowsteps
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight	
	{
		map textures/assault_rock/ground_c07a.jpg
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.1 0.1
	}
	{
		map textures/assault_rock/ground_c09a.jpg
		tcmod scale 0.1 0.1 
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

textures/nerve-guf/trenchterra1_1to2
{
	surfaceparm snowsteps
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight	
	{
		map textures/terrain/grass_ml03cmp.jpg
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.1 0.1 
	}
	{
		map textures/assault_rock/ground_c07a.jpg
		tcmod scale 0.05 0.05 
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

textures/nerve-guf/trenchterra1_1to3
{
	surfaceparm snowsteps
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight	
	{
		map textures/terrain/grass_ml03cmp.jpg
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.1 0.1 
	}
	{
		map textures/terrain/mountain_m09.jpg
		tcmod scale 0.05 0.05 
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

textures/nerve-guf/trenchterra1_1to4
{
	surfaceparm snowsteps
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight	
	{
		map textures/terrain/grass_ml03cmp.jpg
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.1 0.1 
	}
	{
		map textures/assault_rock/ground_c09a.jpg
		tcmod scale 0.1 0.1 
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

textures/nerve-guf/trenchterra1_2to3
{
	surfaceparm snowsteps
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight	
	{
		map textures/assault_rock/ground_c07a.jpg
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.05 0.05 
	}
	{
		map textures/terrain/mountain_m09.jpg
		tcmod scale 0.05 0.05 
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

textures/nerve-guf/trenchterra1_2to4
{
	surfaceparm snowsteps
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight	
	{
		map textures/assault_rock/ground_c07a.jpg
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.05 0.05 
	}
	{
		map textures/assault_rock/ground_c09a.jpg
		tcmod scale 0.1 0.1 
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

textures/nerve-guf/trenchterra1_3to4
{
  surfaceparm snowsteps
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight	
	{
		map textures/terrain/mountain_m09.jpg
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.05 0.05 
	}
	{
		map textures/assault_rock/ground_c09a.jpg
		tcmod scale 0.1 0.1 
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

textures/alpha/tree_c01
{
    surfaceparm metalsteps
    cull disable
    {
        map textures/alpha/tree_c01.tga
        alphaFunc GE128
        depthWrite
        rgbGen vertex
    }
}

textures/alpha/tree_c02
{
    surfaceparm metalsteps
    cull disable
    {
        map textures/alpha/tree_c02.tga
        alphaFunc GE128
        depthWrite
        rgbGen vertex
    }
}

textures/alpha/tree_c03
{
    surfaceparm metalsteps
    cull disable
    {
        map textures/alpha/tree_c03.tga
        alphaFunc GE128
        depthWrite
        rgbGen vertex
    }
}

textures/alpha/tree_test
{
    surfaceparm metalsteps
    cull disable
    {
        map textures/alpha/tree_test.tga
        alphaFunc GE128
        depthWrite
        rgbGen vertex
    }
}

textures/tree/tree_pine1m
{
	surfaceparm metalsteps		
	cull none
	{
		map textures/tree/tree_pine1m.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}

}

textures/tree/tree_m01
{
			
	cull twosided
	{
		map textures/tree/tree_m01.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	//	rgbGen vertex
		depthWrite

	}

}

textures/tree/tree_m03
{
			
	cull twosided
	{
		map textures/tree/tree_m03.tga
	//	alphaFunc GE128
	//	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		blendFunc blend
		rgbGen identity
	//	rgbGen vertex
	//	depthWrite

	}

}

textures/tree/tree_m04
{
			
	cull twosided
	{
		map textures/tree/tree_m04.tga
		alphaFunc GT0
	//	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	//	blendFunc blend
		rgbGen identity
	//	rgbGen vertex
	//	depthWrite

	}

}

textures/tree/tree_m05
{
			
	cull twosided
	{
		map textures/tree/tree_m05.tga
		alphaFunc GE128
	//	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	//	blendFunc blend
		rgbGen identity
	//	rgbGen vertex
	//	depthWrite

	}

}

textures/tree/tree_m06
{
			
	cull twosided
	{
		map textures/tree/tree_m06.tga
		alphaFunc GE128
	//	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	//	blendFunc blend
		rgbGen identity
	//	rgbGen vertex
	//	depthWrite

	}

}

textures/tree/tree_m07
{
	//sort 10		
	surfaceparm alphashadow
	{
		map textures/tree/tree_m07.tga
	//	alphaFunc GE128
	//	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		blendFunc blend
		rgbGen identity
	//	rgbGen vertex
	//	depthWrite

	}

}

textures/tree/tree_m08
{
	//sort 10		
	cull twosided
	surfaceparm alphashadow
	// nopicmip
	// nomipmap
	{

		map textures/tree/tree_m08.tga
		alphaFunc GE128
	//	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	//	blendFunc blend
		rgbGen identity
	//	rgbGen vertex
	//	depthWrite

	}

}



textures/tree/tree_m09
{
	//sort 10		
	cull twosided
	surfaceparm alphashadow
	// nopicmip
	// nomipmap
	{

		map textures/tree/tree_m09.tga
		//blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GE128
		rgbGen identity

	}

}

textures/tree/tree_m01s
{
	//sort 10		
	cull twosided
	{
		map textures/tree/tree_m01s.tga
	//	alphaFunc GE128
	//	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		blendFunc blend
		rgbGen identity
	//	rgbGen vertex
	//	depthWrite

	}

}

textures/tree/tree_m02s
{
	//sort 10		
	cull twosided
	{
		map textures/tree/tree_m02s.tga
	//	alphaFunc GE128
	//	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		blendFunc blend
		rgbGen identity
	//	rgbGen vertex
	//	depthWrite

	}

}

textures/tree/tree_m01t
{
	//sort 10		
	cull twosided
	{
		map textures/tree/tree_m01t.tga
	//	alphaFunc GE128
	//	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		blendFunc blend
		rgbGen identity
	//	rgbGen vertex
	//	depthWrite

	}

}

textures/tree/tree_m02dm
{
	//sort 10		
//	cull twosided
	{
		map textures/tree/tree_m02dm.tga
	//	alphaFunc GE128
	//	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		blendFunc blend
		rgbGen identity
	//	rgbGen vertex
	//	depthWrite

	}

}

textures/tree/tree_m02s_snow
{
	//sort 10		
	cull twosided
	{
		map textures/tree/tree_m02s_snow.tga
	//	alphaFunc GE128
	//	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		blendFunc blend
		rgbGen identity
	//	rgbGen vertex
	//	depthWrite

	}

}

textures/tree/branch_pine2
{
	//sort 10		
	cull twosided
	{
		map textures/tree/branch_pine2.tga
		alphaFunc GE128
		rgbGen identity

	}

}
textures/tree/branch_pine1
{
	//sort 10		
	cull twosided
	{
		map textures/tree/branch_pine1.tga
		alphaFunc GE128
		rgbGen identity

	}

}

textures/tree/branch_pine5
{
	//sort 10		
	cull twosided
	{
		map textures/tree/branch_pine5.tga
		alphaFunc GE128
		rgbGen identity

	}

}

textures/tree/branch_pine5a
{
	//sort 10		
	cull twosided
	{
		map textures/tree/branch_pine5a.tga
		alphaFunc GE128
		rgbGen identity

	}

}
textures/tree/branch_pine6
{
	//sort 10		
	cull twosided
	{
		map textures/tree/branch_pine6.tga
		alphaFunc GE128
		rgbGen identity

	}

}

textures/tree/branch_pine7
{
	//sort 10
	deformVertexes autosprite2		
	cull twosided
	{
		map textures/tree/branch_pine7.tga
	//	alphaFunc GE128
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity

	}

}


textures/tree/trunck3
{
			
	cull twosided
	{
		map textures/tree/trunck3.tga
	//	alphaFunc GE128
	//	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	//	rgbGen vertex
	//	depthWrite

	}

}

textures/tree/trunck3a
{
			
	cull twosided
	{
		map textures/tree/trunck3a.tga
	//	alphaFunc GE128
	//	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	//	rgbGen vertex
	//	depthWrite

	}

}

textures/tree/alpha_back
{
			
	cull twosided
	{
		map textures/tree/alpha_back.tga
	//	alphaFunc GE128
	//	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	//	rgbGen vertex
	//	depthWrite

	}

}

textures/tree/tree_m08_lod
{
	// sort 10		
	cull twosided
	// nopicmip
	// nomipmap
	// deformVertexes autoSprite2
	{

		map textures/tree/tree_m08_lod.tga
		alphaFunc GE128
	//	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	//	blendFunc blend
		rgbGen identity
	//	rgbGen vertex
	//	depthWrite

	}

}

textures/tree/tree_m07_lod
{
	// sort 10		
	cull twosided
	// nopicmip
	// nomipmap
	//deformVertexes autoSprite2

	{

		map textures/tree/tree_m07_lod.tga
		alphaFunc GE128
	//	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	//	blendFunc blend
		rgbGen identity
	//	rgbGen vertex
	//	depthWrite

	}

}

textures/tree/weed_c01
{
		
	cull twosided


	{

		map textures/tree/weed_c01.tga
		alphaFunc GE128
	//	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	//	blendFunc blend
		rgbGen identity
	//	rgbGen vertex
	//	depthWrite

	}

}

textures/tree/weed_c05
{
		
	cull twosided


	{

		map textures/tree/weed_c05.tga
		alphaFunc GE128
	//	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	//	blendFunc blend
		rgbGen identity
	//	rgbGen vertex
	//	depthWrite

	}

}

textures/tree/tree_m08snow
{

	surfaceparm alphashadow
	//sort 10		
	cull twosided
	// nopicmip
	// nomipmap
	{

		map textures/tree/tree_m08snow.tga
		alphaFunc GE128
	//	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	//	blendFunc blend
		rgbGen identity
	//	rgbGen vertex
	//	depthWrite

	}

}

textures/tree/pine_m01
{
	//sort 10		
	cull twosided
	{
		map textures/tree/pine_m01.tga
		alphaFunc GE128
	//	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	//	blendFunc blend
		rgbGen identity
	}

}

textures/tree/branch1
{
	//sort 10		
	cull twosided
	{
		map textures/tree/branch1.tga
		alphaFunc GE128
	//	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	//	blendFunc blend
		rgbGen identity
	}

}

textures/tree/branch2
{
	//sort 10		
	cull twosided
	{
		map textures/tree/branch2.tga
	//	alphaFunc GE128
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	//	blendFunc blend
		rgbGen lightingdiffuse
	}

}

textures/tree/branch3
{
	//sort 10		
	cull twosided
	{
		map textures/tree/branch3.tga
		alphaFunc GE128
	//	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	//	blendFunc blend
		rgbGen lightingdiffuse
	}
}
textures/tree/branch3a
{
	//sort 10		
	cull twosided


	deformVertexes wave 194 sin 0 2 0 .1
	deformVertexes wave 30 sin 0 1 0 .3
	deformVertexes wave 194 sin 0 1 0 .4

	{
		map textures/tree/branch3a.tga
		alphaFunc GE128
	//	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	//	blendFunc blend
		rgbGen identity
	}
}
textures/tree/branch3b
{
	//sort 10		
	cull twosided
	{
		map textures/tree/branch3b.tga
		alphaFunc GE128
	//	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	//	blendFunc blend
		rgbGen lightingdiffuse
	}

}
textures/alpha/bench_c01
{
    surfaceparm metalsteps
    nomipmaps
    nopicmip
    cull disable
    {
        map textures/alpha/chateaubench_c01.tga
        alphaFunc GE128
        depthWrite
        rgbGen vertex
    }
}
textures/alpha/bench_c02
{
    surfaceparm metalsteps
    nomipmaps
    nopicmip
    cull disable
    {
        map textures/alpha/chateaubench_c02.tga
        alphaFunc GE128
        depthWrite
        rgbGen vertex
    }
}

textures/training/trees_m01
{
    surfaceparm alphashadow
    {  
        map textures/training/trees_m01.tga
	  blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//      alphaFunc GE128
        depthWrite
        rgbGen identity

    }
}

textures/sleepy/tree_c01
{
    surfaceparm metalsteps
    cull disable
    {
        map textures/sleepy/tree_c01.tga
        alphaFunc GE128
        depthWrite
        rgbGen vertex
    }
}
textures/sleepy/grouptree_c01
{
    surfaceparm metalsteps
    cull disable
    {
        map textures/sleepy/grouptree_c01.tga
        alphaFunc GE128
        depthWrite
        rgbGen vertex
    }
}
textures/tree/snowtree_c01
{
		
	cull twosided
    surfaceparm alphashadow
	{
		map textures/tree/snowtree_c01.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}

}
textures/tree/snowtree_c01a
{
			
	cull twosided
    surfaceparm alphashadow
	{
		map textures/tree/snowtree_c01a.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}

}
textures/tree/snowtree_c02
{
			
	cull twosided
    surfaceparm alphashadow
	{
		map textures/tree/snowtree_c02.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}

}
textures/tree/snowtree_c02a
{
		
	cull twosided
    surfaceparm alphashadow
	{
		map textures/tree/snowtree_c02a.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}

}
textures/tree/snowtree_c03
{
		
	cull twosided
    surfaceparm alphashadow
	{
		map textures/tree/snowtree_c03.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}

}
textures/tree/snowtree_c03a
{
		
	cull twosided
    surfaceparm alphashadow
	{
		map textures/tree/snowtree_c03a.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}

}
textures/tree/snowtree_c04
{
			
	cull twosided
    surfaceparm alphashadow
	{
		map textures/tree/snowtree_c04.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}

}
textures/tree/snowtree_c04a
{
			
	cull twosided
    surfaceparm alphashadow
	{
		map textures/tree/snowtree_c04a.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}

}
textures/tree/tree_c01
{
			
	cull twosided
    surfaceparm alphashadow
	{
		map textures/tree/tree_c01.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
		depthWrite

	}

}
textures/tree/ivy_c01
{
	surfaceparm alphashadow
	cull twosided
	{
		map textures/tree/ivy_c01.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
		depthWrite

	}

}
textures/tree/ivy_c01a
{
	    surfaceparm alphashadow		
	cull twosided
	{
		map textures/tree/ivy_c01a.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
		depthWrite

	}

}
textures/tree/ivy_c01b
{
	    surfaceparm alphashadow		
	cull twosided
	{
		map textures/tree/ivy_c01b.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
		depthWrite

	}

}
textures/tree/ivy_c02
{
	    surfaceparm alphashadow		
	cull twosided
	{
		map textures/tree/ivy_c02.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
		depthWrite

	}

}
textures/tree/ivy_c02a
{
	    surfaceparm alphashadow		
	cull twosided
	{
		map textures/tree/ivy_c02a.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
		depthWrite

	}

}
textures/tree/ivy_c02b
{
	    surfaceparm alphashadow		
	cull twosided
	{
		map textures/tree/ivy_c02b.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
		depthWrite

	}

}

textures/tree/ivy_c03
{
    surfaceparm alphashadow			
	cull twosided
	{
		map textures/tree/ivy_c03.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
		depthWrite

	}

}
textures/tree/ivy_c03a
{
    surfaceparm alphashadow			
	cull twosided
	{
		map textures/tree/ivy_c03a.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
		depthWrite

	}

}
textures/tree/ivy_c03b
{
    surfaceparm alphashadow			
	cull twosided
	{
		map textures/tree/ivy_c03b.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
		depthWrite

	}

}

textures/tree/ivy_c04
{
    surfaceparm alphashadow			
	cull twosided
	{
		map textures/tree/ivy_c04.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
		depthWrite

	}

}
textures/tree/ivy_c04a
{
    surfaceparm alphashadow			
	cull twosided
	{
		map textures/tree/ivy_c04a.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
		depthWrite

	}

}
textures/tree/ivy_c04b
{
    surfaceparm alphashadow			
	cull twosided
	{
		map textures/tree/ivy_c04b.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
		depthWrite

	}

}
textures/tree/ivy_c05
{
    surfaceparm alphashadow			
	cull twosided
	{
		map textures/tree/ivy_c05.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
		depthWrite

	}

}


textures/tree/ivy_c05a
{
    surfaceparm alphashadow			
	cull twosided
	{
		map textures/tree/ivy_c05a.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
		depthWrite

	}

}
textures/tree/ivy_c05b
{
    surfaceparm alphashadow			
	cull twosided
	{
		map textures/tree/ivy_c05b.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
		depthWrite

	}

}
textures/tree/ivy_c06
{
    surfaceparm alphashadow			
	cull twosided
	{
		map textures/tree/ivy_c06.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
		depthWrite

	}

}
textures/tree/ivy_c07
{
    surfaceparm alphashadow			
	cull twosided
	{
		map textures/tree/ivy_c07.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
		depthWrite

	}

}
textures/tree/ivy_c08
{
    surfaceparm alphashadow			
	cull twosided
	{
		map textures/tree/ivy_c058.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
		depthWrite

	}

}
textures/tree/ivy_c09
{
    surfaceparm alphashadow			
	cull twosided
	{
		map textures/tree/ivy_c09.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
		depthWrite

	}

}

// maxx tree test ////
	//GL_ONE_MINUS_SRC_ALPHA

	//
	//

models/mapobjects/tree/branch3a
{
	surfaceparm alphashadow


	deformVertexes wave 194 sin 0 2 0 .1
	deformVertexes wave 30 sin 0 1 0 .3
	deformVertexes wave 194 sin 0 1 0 .4

	cull twosided

	{
            map models/mapobjects/tree/branch3.tga


		alphaFunc GE128
		depthWrite
		rgbGen vertex

	}
}


models/mapobjects/tree/branch4
{
	surfaceparm alphashadow


	deformVertexes wave 194 sin 0 2 0 .1
	deformVertexes wave 30 sin 0 1 0 .3
	deformVertexes wave 194 sin 0 1 0 .4

	cull twosided

	{
            map models/mapobjects/tree/branch4.tga


		alphaFunc GE128
		depthWrite
		rgbGen vertex

	}
}


////c test////
models/mapobjects/test/tree
{
	surfaceparm alphashadow


	deformVertexes wave 194 sin 0 2 0 .1
	deformVertexes wave 30 sin 0 1 0 .3
	deformVertexes wave 194 sin 0 1 0 .4

	cull twosided

	{
            map models/mapobjects/test/tree.tga


		alphaFunc GE128
		depthWrite
		rgbGen vertex

	}
}

models/mapobjects/tree/branch5
{
	surfaceparm alphashadow


	deformVertexes wave 194 sin 0 2 0 .1
	deformVertexes wave 30 sin 0 1 0 .3
	deformVertexes wave 194 sin 0 1 0 .4

	cull twosided

	{
            map models/mapobjects/tree/branch5.tga


		alphaFunc GE128
		depthWrite
		rgbGen vertex

	}
}


models/mapobjects/tree/branch6
{
	surfaceparm alphashadow


	deformVertexes wave 194 sin 0 2 0 .1
	deformVertexes wave 30 sin 0 1 0 .3
	deformVertexes wave 194 sin 0 1 0 .4

	cull twosided

	{
            map models/mapobjects/tree/branch6.tga


		alphaFunc GE128
		depthWrite
		rgbGen vertex

	}
}

models/mapobjects/tree/pine3
{
	surfaceparm alphashadow


	deformVertexes wave 194 sin 0 2 0 .1
	deformVertexes wave 30 sin 0 1 0 .3
	deformVertexes wave 194 sin 0 1 0 .4

	cull twosided

	{
            map models/mapobjects/tree/pine3.tga


		alphaFunc GE128
		depthWrite
		rgbGen identity

	}
}

models/mapobjects/tree/branch8
{
	surfaceparm alphashadow


	deformVertexes wave 194 sin 0 2 0 .1
	deformVertexes wave 30 sin 0 1 0 .3
	deformVertexes wave 194 sin 0 1 0 .4

	cull twosided

	{
            map models/mapobjects/tree/branch8.tga


		alphaFunc GE128
		depthWrite
		rgbGen vertex

	}
}


models/mapobjects/tree/trunck3
{
	{
		map models/mapobjects/tree/trunck3.tga
		rgbGen identity	
	}

}
models/mapobjects/tree/branch
{
     cull disable
     deformVertexes wave 100 sin 0 .1 0 .2
      surfaceparm	trans
        {
                map models/mapobjects/tree/branch.tga
                alphaFunc GE128
		depthWrite
		//rgbGen identity
                //rgbGen lightingDiffuse
                rgbGen vertex
        }


}

models/mapobjects/tree/tree_vil1
{
	surfaceparm alphashadow
	deformVertexes wave 194 sin 0 2 0 .1
	deformVertexes wave 30 sin 0 1 0 .3
	deformVertexes wave 194 sin 0 1 0 .4
	cull twosided


	{
		map models/mapobjects/tree/tree_vil1.tga
		alphaFunc GE128
		rgbGen vertex
	}

}

models/mapobjects/tree/branch_slp1
{
	surfaceparm alphashadow
//	deformVertexes wave 194 sin 0 2 0 .1
//	deformVertexes wave 30 sin 0 1 0 .3
//	deformVertexes wave 194 sin 0 1 0 .4
	cull twosided


	{
		map models/mapobjects/tree/branch_slp1.tga
		alphaFunc GE128
		rgbGen vertex
	}

}

models/mapobjects/tree/branch_slp2
{
	surfaceparm alphashadow
//	deformVertexes wave 194 sin 0 2 0 .1
//	deformVertexes wave 30 sin 0 1 0 .3
//	deformVertexes wave 194 sin 0 1 0 .4
	cull twosided


	{
		map models/mapobjects/tree/branch_slp2.tga
		alphaFunc GE128
		rgbGen vertex
	}

}

models/mapobjects/tree/trunck_2a
{
	surfaceparm alphashadow
	cull twosided


	{
		map models/mapobjects/tree/trunck_2a.tga
//		alphaFunc GE128
		rgbGen vertex
	}

}

models/mapobjects/tree/branch_vil1
{
	surfaceparm alphashadow
//	deformVertexes wave 194 sin 0 2 0 .1
//	deformVertexes wave 30 sin 0 1 0 .3
//	deformVertexes wave 194 sin 0 1 0 .4
	cull twosided


	{
		map models/mapobjects/tree/branch_vil1.tga
		alphaFunc GE128
		rgbGen lightingdiffuse
	}

}

models/mapobjects/tree/branch_vil1a
{
	surfaceparm alphashadow
//	deformVertexes wave 194 sin 0 2 0 .1
//	deformVertexes wave 30 sin 0 1 0 .3
//	deformVertexes wave 194 sin 0 1 0 .4
	cull twosided


	{
		map models/mapobjects/tree/branch_vil1a.tga
		alphaFunc GE128
		rgbGen lightingdiffuse
	}

}

models/mapobjects/tree/branch_vil1_d
{
	surfaceparm alphashadow
//	deformVertexes wave 194 sin 0 2 0 .1
//	deformVertexes wave 30 sin 0 1 0 .3
//	deformVertexes wave 194 sin 0 1 0 .4
	cull twosided


	{
		map models/mapobjects/tree/branch_vil1.tga
		alphaFunc GE128
		rgbGen identity
	}

}

models/mapobjects/tree/branch_vil1a_d
{
	surfaceparm alphashadow
//	deformVertexes wave 194 sin 0 2 0 .1
//	deformVertexes wave 30 sin 0 1 0 .3
//	deformVertexes wave 194 sin 0 1 0 .4
	cull twosided


	{
		map models/mapobjects/tree/branch_vil1a.tga
		alphaFunc GE128
		rgbGen identity
	}

}

models/mapobjects/plants/vine1
{
	surfaceparm alphashadow
//	deformVertexes wave 194 sin 0 2 0 .1
//	deformVertexes wave 30 sin 0 1 0 .3
//	deformVertexes wave 194 sin 0 1 0 .4
	cull twosided


	{
		map models/mapobjects/plants/vine1.tga
	//	alphaFunc GE128
		blendfunc blend
		rgbGen lightingdiffuse
	}

}

models/mapobjects/plants/bushes1
{
	surfaceparm alphashadow
//	deformVertexes wave 194 sin 0 2 0 .1
//	deformVertexes wave 30 sin 0 1 0 .3
//	deformVertexes wave 194 sin 0 1 0 .4
	cull twosided


	{
		map models/mapobjects/plants/bushes1.tga
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
models/mapobjects/plants/small_branch
{
	surfaceparm alphashadow
	deformVertexes wave 194 sin 0 2 0 .1
	deformVertexes wave 30 sin 0 1 0 .1
	deformVertexes wave 194 sin 0 1 0 .1
	cull twosided


	{
		map models/mapobjects/plants/small_branch.tga
		alphaFunc GE128
		rgbGen lightingdiffuse
//		rgbGen identity
	}

}

models/mapobjects/tree/trunck2l
{
	surfaceparm alphashadow
//	deformVertexes wave 194 sin 0 2 0 .1
//	deformVertexes wave 30 sin 0 1 0 .3
//	deformVertexes wave 194 sin 0 1 0 .4
	cull twosided


	{
		map models/mapobjects/tree/trunck2l.tga
		rgbGen lightingdiffuse
//		rgbGen identity
	}

}

models/mapobjects/tree/trunck2al
{

	{
		map models/mapobjects/tree/trunck2al.tga
		rgbGen identity

	}

}

models/mapobjects/tree/trunk_trn1
{

	{
		map models/mapobjects/tree/trunk_trn1.tga
		rgbGen identity

	}

}

models/mapobjects/tree/trunk_trn2
{

	{
		map models/mapobjects/tree/trunk_trn2.tga
		rgbGen identity

	}

}

models/mapobjects/plants/small_branch_t
{
	surfaceparm alphashadow
//	deformVertexes wave 194 sin 0 2 0 .1
	deformVertexes wave 30 sin 0 1 0 .1
	deformVertexes wave 194 sin 0 1 0 .1
	cull twosided


	{
		map models/mapobjects/plants/small_branch_t.tga
		alphaFunc GE128
//		rgbGen lightingdiffuse
		rgbGen identity
	}

}

models/mapobjects/tree/tree_vil2t
{
	surfaceparm alphashadow
//	deformVertexes wave 194 sin 0 2 0 .1
	deformVertexes wave 30 sin 0 1 0 .1
	deformVertexes wave 194 sin 0 1 0 .1
	cull twosided


	{
		map models/mapobjects/tree/tree_vil2t.tga
		alphaFunc GE128
//		rgbGen lightingdiffuse
		rgbGen identity
	}

}

models/mapobjects/tree/branch_vil1at
{
	surfaceparm alphashadow
//	deformVertexes wave 194 sin 0 2 0 .1
	deformVertexes wave 30 sin 0 1 0 .1
	deformVertexes wave 194 sin 0 1 0 .1
	cull twosided


	{
		map models/mapobjects/tree/branch_vil1at.tga
		alphaFunc GE128
//		rgbGen lightingdiffuse
		rgbGen identity
	}

}


models/mapobjects/tree/branch_vil1t
{
	surfaceparm alphashadow
//	deformVertexes wave 194 sin 0 2 0 .1
	deformVertexes wave 30 sin 0 1 0 .1
	deformVertexes wave 194 sin 0 1 0 .1
	cull twosided


	{
		map models/mapobjects/tree/branch_vil1t.tga
		alphaFunc GE128
//		rgbGen lightingdiffuse
		rgbGen identity
	}

}

models/mapobjects/snow_tree/stree_f1
{
	surfaceparm alphashadow
	cull twosided


	{
		map models/mapobjects/snow_tree/stree_f1.tga
//		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GE128
		rgbGen lightingdiffuse
	}

}


models/mapobjects/snow_tree/stree_s1
{
	surfaceparm alphashadow
	cull twosided


	{
		map models/mapobjects/snow_tree/stree_s1.tga
//		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GE128
		rgbGen lightingdiffuse
	}

}

textures/props/bags_m01
{
	{
		map textures/props/bags_m01.tga
		rgbGen vertex	
	}
}

//
//
//

textures/metal_misc/ametal_m03dm
{
	surfaceparm metalsteps
	{
		map $lightmap
		rgbGen identity
	}
	{
		//map textures/metal_misc/ametal_m03dm.tga
		map textures/metal_misc/ametal_m03a.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/metal_misc/ametal_m03a
{
	surfaceparm metalsteps
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/metal_misc/ametal_m03a.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/metal_misc/ametal_m06a
{
	surfaceparm metalsteps
	{
		map $lightmap
		rgbGen identity
	}
	{
		//map textures/metal_misc/ametal_m06a.tga
		map textures/metal_misc/ametal_m03a.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/nerve-guf/mountain_m09
{
  surfaceparm snowsteps
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight	
	{
		map textures/terrain/mountain_m09.jpg
		tcmod scale 0.05 0.05 
		rgbGen vertex
	}
}

textures/nerve-guf/ground_c09b
{
  surfaceparm snowsteps
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight	
	{
		map textures/assault_rock/ground_c09b.tga
		tcmod scale 0.05 0.05 
		rgbGen vertex
	}
}

textures/assault_rock/ground_c09b
{
  surfaceparm snowsteps
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight	
	{
		//map textures/assault_rock/ground_c09b.tga
		map textures/stone/mxrock3h_snow.tga
		tcmod scale 0.05 0.05 
		rgbGen vertex
	}
}



