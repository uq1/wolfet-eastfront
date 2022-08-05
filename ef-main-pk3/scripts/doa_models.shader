// DOA custom models - FireFly

models/doa/search_light/s_light_main
{ 	qer_editorimage models/doa/search_light/s_light_main.tga
  	implicitMap models/doa/search_light/s_light_main.tga
}

models/doa/search_light/s_light_lamp
{ 	qer_editorimage models/doa/search_light/s_light_lamp.tga
  	implicitMap models/doa/search_light/s_light_lamp.tga
}

models/doa/doa_cabinet/ammo_crate
{
	qer_editorimage map models/doa/doa_cabinet/ammo_outside.tga
    	//cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/doa/doa_cabinet/ammo_outside.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/doa/doa_cabinet/health_crate
{
	qer_editorimage map models/doa/doa_cabinet/health_outside.tga
    	//cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/doa/doa_cabinet/health_outside.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/doa/doa_cabinet/ammo_inside
{
	qer_editorimage map models/doa/doa_cabinet/ammo_inside.tga
    	cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/doa/doa_cabinet/ammo_inside.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/doa/doa_cabinet/health_inside
{
	qer_editorimage models/doa/doa_cabinet/health_inside.tga
    	//cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/doa/doa_cabinet/health_inside.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}


