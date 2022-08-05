//
// Barrel
//

models/furniture/barrel/barrel_m01
{
	qer_editorimage map models/doa/doa_cabinet/ammo_inside.tga
    	cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/furniture/barrel/barrel_m01.jpg
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

//
// Planes
//

models/mapobjects/vehicles/m109b
{
	qer_editorimage map models/doa/doa_cabinet/ammo_inside.tga
    	cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/mapobjects/vehicles/m109b.jpg
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/mapobjects/vehicles/natter
{
	qer_editorimage map models/doa/doa_cabinet/ammo_inside.tga
    	cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/mapobjects/vehicles/natter.jpg
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/mapobjects/vehicles/propeller1-broken
{
	qer_editorimage map models/doa/doa_cabinet/ammo_inside.tga
    	cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/mapobjects/vehicles/propeller1.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/mapobjects/vehicles/propeller1
{
     cull disable
//   surfaceparm alphashadow

        {
        	map models/mapobjects/vehicles/propeller1.tga
        	
			blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
			// depthWrite
			alphaFunc GT0
			rgbGen lightingdiffuse
        }
}

models/mapobjects/vehicles/vtol3
{
	qer_editorimage map models/doa/doa_cabinet/ammo_inside.tga
    	cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/mapobjects/vehicles/vtol3.jpg
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

//
// Tank - zp_m1_tiger2
//

models/mapobjects/zp_m1_tiger2/zp_m1_chs_snow_dest
{
	qer_editorimage map models/doa/doa_cabinet/ammo_inside.tga
    	cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/mapobjects/zp_m1_tiger2/zp_m1_chs_snow_dest.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/mapobjects/zp_m1_tiger2/zp_m1_cn_snow_dest
{
	qer_editorimage map models/doa/doa_cabinet/ammo_inside.tga
    	cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/mapobjects/zp_m1_tiger2/zp_m1_cn_snow_dest.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/mapobjects/zp_m1_tiger2/zp_m1_trk_dest
{
	qer_editorimage map models/doa/doa_cabinet/ammo_inside.tga
    	cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/mapobjects/zp_m1_tiger2/zp_m1_trk_dest.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/mapobjects/zp_m1_tiger2/zp_m1_trkb_dest
{
	qer_editorimage map models/doa/doa_cabinet/ammo_inside.tga
    	cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/mapobjects/zp_m1_tiger2/zp_m1_trkb_dest.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/mapobjects/zp_m1_tiger2/zp_m1_trks_dest
{
	qer_editorimage map models/doa/doa_cabinet/ammo_inside.tga
    	cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/mapobjects/zp_m1_tiger2/zp_m1_trks_dest.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/mapobjects/zp_m1_tiger2/zp_m1_tur_snow_dest
{
	qer_editorimage map models/doa/doa_cabinet/ammo_inside.tga
    	cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/mapobjects/zp_m1_tiger2/zp_m1_tur_snow_dest.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/mapobjects/zp_m1_tiger2/zp_m1_whl_dest
{
	qer_editorimage map models/doa/doa_cabinet/ammo_inside.tga
    	cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/mapobjects/zp_m1_tiger2/zp_m1_whl_dest.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

//
// Other Tanks
//

models/mapobjects/tanks_sd/churchill_flat_oasis
{
	qer_editorimage map models/doa/doa_cabinet/ammo_inside.tga
    	cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/mapobjects/tanks_sd/churchill_flat_oasis.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/mapobjects/tanks_sd/churchill_flat_temperate
{
	qer_editorimage map models/doa/doa_cabinet/ammo_inside.tga
    	cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/mapobjects/tanks_sd/churchill_flat_temperate.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/mapobjects/tanks_sd/jagdpanther_additions_desert
{
	qer_editorimage map models/doa/doa_cabinet/ammo_inside.tga
    	cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/mapobjects/tanks_sd/jagdpanther_additions_desert.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/mapobjects/tanks_sd/jagdpanther_additions_snow
{
	qer_editorimage map models/doa/doa_cabinet/ammo_inside.tga
    	cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/mapobjects/tanks_sd/jagdpanther_additions_snow.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/mapobjects/tanks_sd/jagdpanther_additions_temperate
{
	qer_editorimage map models/doa/doa_cabinet/ammo_inside.tga
    	cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/mapobjects/tanks_sd/jagdpanther_additions_temperate.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/mapobjects/tanks_sd/jagdpanther_full
{
	qer_editorimage map models/doa/doa_cabinet/ammo_inside.tga
    	cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/mapobjects/tanks_sd/jagdpanther_full.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/mapobjects/tanks_sd/jagdpanther_full_temperate
{
	qer_editorimage map models/doa/doa_cabinet/ammo_inside.tga
    	cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/mapobjects/tanks_sd/jagdpanther_full_temperate.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/mapobjects/tanks_sd/jagdpanther_snow
{
	qer_editorimage map models/doa/doa_cabinet/ammo_inside.tga
    	cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/mapobjects/tanks_sd/jagdpanther_snow.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/mapobjects/tanks_sd/mg42turret_2
{
	qer_editorimage map models/doa/doa_cabinet/ammo_inside.tga
    	cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/mapobjects/tanks_sd/mg42turret_2.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/mapobjects/tanks_sd/tracks
{
	qer_editorimage map models/doa/doa_cabinet/ammo_inside.tga
    	cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/mapobjects/tanks_sd/tracks.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/mapobjects/tanks_sd/tracks_temperate
{
	qer_editorimage map models/doa/doa_cabinet/ammo_inside.tga
    	cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/mapobjects/tanks_sd/tracks_temperate.jpg
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/players/model/atgun
{
	qer_editorimage map models/doa/doa_cabinet/ammo_inside.tga
    	cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/players/model/atgun.jpg
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/players/model/atgun.jpg
{
	qer_editorimage map models/doa/doa_cabinet/ammo_inside.tga
    	cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/players/model/atgun.jpg
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/players/model/kv1
{
	qer_editorimage map models/doa/doa_cabinet/ammo_inside.tga
    	cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/players/model/kv1.jpg
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/players/model/kv1.jpg
{
	qer_editorimage map models/doa/doa_cabinet/ammo_inside.tga
    	cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/players/model/kv1.jpg
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/players/model/panzeriv
{
	qer_editorimage map models/doa/doa_cabinet/ammo_inside.tga
    	cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/players/model/panzeriv.JPG
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/players/model/panzeriv.jpg
{
	qer_editorimage map models/doa/doa_cabinet/ammo_inside.tga
    	cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/players/model/panzeriv.JPG
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/players/model/t34
{
	qer_editorimage map models/doa/doa_cabinet/ammo_inside.tga
    	cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/players/model/t34.jpg
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/players/model/t34.jpg
{
	qer_editorimage map models/doa/doa_cabinet/ammo_inside.tga
    	cull twosided
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/players/model/t34.jpg
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

