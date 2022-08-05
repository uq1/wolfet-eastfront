textures/UPPER_X/terrain_base
{
	nopicmips
	q3map_lightmapaxis z
	q3map_lightmapmergable
	q3map_tcGen ivector ( 512 0 0 ) ( 0 512 0 )
}

textures/UPPER_X/terrain_0
{
	q3map_baseshader textures/UPPER_X/terrain_base
	qer_editorimage textures/stalin_terrain/snow_var01_big.tga
	surfaceparm landmine
	surfaceparm snowsteps
	{
		map textures/stalin_terrain/snow_var01_big.tga
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
}

textures/UPPER_X/terrain_1
{
	q3map_baseshader textures/UPPER_X/terrain_base
	qer_editorimage textures/stalin_terrain/snow_muddy.jpg
	surfaceparm landmine
	surfaceparm snowsteps
	{
		map textures/stalin_terrain/snow_muddy.jpg
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
}

textures/UPPER_X/terrain_2
{
	q3map_baseshader textures/UPPER_X/terrain_base
	qer_editorimage textures/stalin_terrain/s_dirt_m03i_2_big.tga
	surfaceparm landmine
	surfaceparm snowsteps
	{
		map textures/stalin_terrain/s_dirt_m03i_2_big.tga
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
}

textures/UPPER_X/terrain_3
{
	q3map_baseshader textures/UPPER_X/terrain_base
	qer_editorimage textures/stalin_terrain/rocky_sand.tga
	surfaceparm landmine
	surfaceparm snowsteps
	{
		map textures/stalin_terrain/rocky_sand.tga
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
}

textures/UPPER_X/terrain_0to1
{
	q3map_baseshader textures/UPPER_X/terrain_base
	qer_editorimage textures/stalin_terrain/snow_var01_big.tga
	surfaceparm landmine
	surfaceparm snowsteps
	{
		map textures/stalin_terrain/snow_var01_big.tga
	}
	{
		map textures/stalin_terrain/snow_muddy.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
}

textures/UPPER_X/terrain_0to2
{
	q3map_baseshader textures/UPPER_X/terrain_base
	qer_editorimage textures/stalin_terrain/snow_var01_big.tga
	surfaceparm landmine
	surfaceparm snowsteps
	{
		map textures/stalin_terrain/snow_var01_big.tga
	}
	{
		map textures/stalin_terrain/s_dirt_m03i_2_big.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
}

textures/UPPER_X/terrain_0to3
{
	q3map_baseshader textures/UPPER_X/terrain_base
	qer_editorimage textures/stalin_terrain/snow_var01_big.tga
	surfaceparm landmine
	surfaceparm snowsteps
	{
		map textures/stalin_terrain/snow_var01_big.tga
	}
	{
		map textures/stalin_terrain/rocky_sand.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
}

textures/UPPER_X/terrain_1to2
{
	q3map_baseshader textures/UPPER_X/terrain_base
	qer_editorimage textures/stalin_terrain/snow_muddy.jpg
	surfaceparm landmine
	surfaceparm snowsteps
	{
		map textures/stalin_terrain/snow_muddy.jpg
	}
	{
		map textures/stalin_terrain/s_dirt_m03i_2_big.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
}

textures/UPPER_X/terrain_1to3
{
	q3map_baseshader textures/UPPER_X/terrain_base
	qer_editorimage textures/stalin_terrain/snow_muddy.jpg
	surfaceparm landmine
	surfaceparm snowsteps
	{
		map textures/stalin_terrain/snow_muddy.jpg
	}
	{
		map textures/stalin_terrain/rocky_sand.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
}

textures/UPPER_X/terrain_2to3
{
	q3map_baseshader textures/UPPER_X/terrain_base
	qer_editorimage textures/stalin_terrain/s_dirt_m03i_2_big.tga
	surfaceparm landmine
	surfaceparm snowsteps
	{
		map textures/stalin_terrain/s_dirt_m03i_2_big.tga
	}
	{
		map textures/stalin_terrain/rocky_sand.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
}