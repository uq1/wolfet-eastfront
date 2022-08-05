// Creator: VoYtHAs
//
// www.etmaps.glt.pl

// many modf by mlyn3k

textures/exodus_sky/exodus_S
{
	nocompress
	qer_editorimage textures/exodus/dsz.jpg
	// q3map_lightrgb 0.8 0.9 1.0
      q3map_lightrgb 0.8 0.9 1.0
	// q3map_sun 1 .96 .9 165 165 18
	q3map_sun 1 .96 .9 30 175 18
	// q3map_skylight 60 3
	q3map_skylight 45 3
	//q3map_surfacelight 25
	q3map_surfacelight 25 //20
	q3map_nofog
	
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nodlight
	surfaceparm sky

	skyparms textures/skies_sd/wurzburg_env/sky 512 -

	sunshader textures/skies_sd/batterysunfog
 
	{	fog off
		map textures/exodus/exodus_sky.jpg
		tcMod scale 2.5 2.5
		tcMod scroll 0.0015 -0.003
		rgbGen identityLighting
	}

	{
		fog off
		clampmap textures/exodus/exodus_mask.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		tcMod scale 0.956 0.956
		tcMod transform 1 0 0 1 -1 -1
		rgbGen identityLighting
		//rgbGen const ( 0.6 0.6 0.6 ) 
		rgbGen const ( 0.4 0.4 0.6 ) 
	}
	  
}

//textures/exodus/fog
//{
//qer_editorimage textures/sfx/fog_grey1.tga

//surfaceparm nodraw
//surfaceparm nosolid
//surfaceparm trans
//surfaceparm fog

//fogparms (0.09411 0.09803 0.12549) 6000
//}
