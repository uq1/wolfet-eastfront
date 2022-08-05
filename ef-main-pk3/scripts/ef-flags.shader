//models/multiplayer/flagpole/german
//{
//	nocompress
//      nopicmip
//	surfaceparm trans 
//	surfaceparm alphashadow
//	cull none
//	{
//		map models/multiplayer/flagpole/german.tga
//		blendfunc blend
//		alphaFunc GE128 
//		depthWrite 
//		rgbGen lightingDiffuse
//	}
//}

models/multiplayer/flagpole/german
{
  nocompress
  nopicmip
	surfaceparm trans 
	cull none
	{
		map models/multiplayer/flagpole/german.tga
		blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		//blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		//rgbGen lightingdiffuse
		//blendfunc add
	}
}

//models/multiplayer/flagpole/american
//{
//	nocompress
//     nopicmip
//	surfaceparm trans 
//	surfaceparm alphashadow
//	cull none
//	{
//		map models/multiplayer/flagpole/american.tga
//		blendfunc blend 
//		alphaFunc GE128 
//		depthWrite 
//		alphaGen lightingDiffuse
//	}
//}

models/multiplayer/flagpole/american
{
  nocompress
  nopicmip
	surfaceparm trans 
	cull none
	{
		map models/multiplayer/flagpole/american.tga
		blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		//blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		//rgbGen lightingdiffuse
		//blendfunc add
	}
}

models/mapobjects/flag/flag_dam.jpg
{
	nocompress
      nopicmip
	surfaceparm trans 
	surfaceparm alphashadow
	cull none
	{
		mapmodels/mapobjects/flag/flag_dam.tga
		blendfunc blend 
		alphaFunc GE128 
		depthWrite 
		alphaGen lightingDiffuse
	}
}
