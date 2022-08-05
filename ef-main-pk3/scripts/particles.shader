//shaders made specifically for particle systems

//yellow spark
y_spark
{
	cull none
	entityMergable		// allow all the sprites to be merged together
	{
		map sprites/y_spark.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen		vertex
		alphaGen	vertex
	}
}

//blue-white spark
b_spark
{
	cull none
	entityMergable		// allow all the sprites to be merged together
	{
		map sprites/b_spark.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen		vertex
		alphaGen	vertex
	}
}
//mud or oil
mud
{
	cull none
	entityMergable		// allow all the sprites to be merged together
	{
		map sprites/mud.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen		vertex
		alphaGen	vertex
	}
}
//mist
p_mistORIG
{
	cull none
	entityMergable		// allow all the sprites to be merged together
	{
		map sprites/mist.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen		vertex
		alphaGen	vertex
	}
}
p_mist
{
	cull none
	nofog
	{
		map sprites/mist.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen		vertex
		alphaGen	vertex
		//blendFunc GL_ZERO GL_ONE_MINUS_SRC_ALPHA
		//alphaGen const 0.2
	}
}
//blue water
spurtORIG
{
	cull none
	entityMergable		// allow all the sprites to be merged together
	{
		map sprites/water_spurt.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen		vertex
		alphaGen	vertex
	}
}
spurt
{
	cull none
	nofog
	{
		map sprites/water_spurt.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen		vertex
		alphaGen	vertex
		//blendFunc GL_ZERO GL_ONE_MINUS_SRC_ALPHA
		//alphaGen const 0.2
	}
}

poisonCloudORIG
{
	cull none
	//entityMergable		// allow all the sprites to be merged together
	qer_editorimage gfx/misc/poisoncloud1.tga
	cull none
	nofog
	surfaceparm nomarks
	surfaceparm nonsolid
	surfaceparm pointlight
	surfaceparm trans
	{
		animmap 5 gfx/misc/poisoncloud1.tga gfx/misc/poisoncloud2.tga gfx/misc/poisoncloud3.tga gfx/misc/poisoncloud4.tga gfx/misc/poisoncloud5.tga 
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen		vertex
		alphaGen	vertex
	}
	{
		map gfx/misc/poisoncloud1.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen		vertex
		alphaGen	vertex
	}
}

poisonCloud
{
	cull none
	entityMergable
	nofog
	{
		map gfx/misc/poisoncloud1.tga
		//map gfx/misc/smokepuff.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

poisonCloud1
{
	cull none
	entityMergable
	nofog
	{
		map gfx/misc/poisoncloud1.tga
		//map gfx/misc/smokepuff.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

poisonCloud2
{
	cull none
	entityMergable
	nofog
	{
		map gfx/misc/poisoncloud2.tga
		//map gfx/misc/smokepuff.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

poisonCloud3
{
	cull none
	entityMergable
	nofog
	{
		map gfx/misc/poisoncloud3.tga
		//map gfx/misc/smokepuff.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

poisonCloud4
{
	cull none
	entityMergable
	nofog
	{
		map gfx/misc/poisoncloud4.tga
		//map gfx/misc/smokepuff.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

poisonCloud5
{
	cull none
	entityMergable
	nofog
	{
		map gfx/misc/poisoncloud5.tga
		//map gfx/misc/smokepuff.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

smokepuff
{
	cull none
	entityMergable
	nofog
	{
		map gfx/misc/smokepuff.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

