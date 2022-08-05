test_light
{
	cull none
	entityMergable
	nocompress
	nofog
//	nomip
	{
		map gfx\misc\muzzleflare.tga
		blendFunc GL_SRC_ALPHA GL_ONE
//		blendFunc GL_ONE GL_ONE
		rgbGen 	vertex
		alphaGen vertex
	}
}

test_light2
{
	cull none
	{
		map gfx/misc/muzzleflare.tga
		blendFunc GL_ONE GL_ONE
		rgbGen vertex
	}
}

test_light3
{
	cull none
	entityMergable
	{
		map gfx/misc/muzzleflare.tga
		blendFunc GL_SRC_ALPHA GL_ONE
		rgbGen vertex
	}
}

test_light4
{
	cull none
	{
		map gfx/misc/muzzleflare.tga
		blendFunc GL_ONE GL_ONE
		rgbGen vertex
	}
}
