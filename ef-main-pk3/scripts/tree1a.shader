// tree common winter (tree1a)
models/mapobjects/coroner/tree1a_trunk
{
	q3map_clipmodel
	surfaceparm woodsteps
	surfaceparm nolightmap
	
	{
		map models/mapobjects/coroner/tree1a_trunk.tga
		rgbGen vertex
	}
}

models/mapobjects/coroner/tree1a_branch
{
	qer_alphafunc greater 0.5
	
	cull disable
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm alphashadow
	surfaceparm nolightmap
	
	//q3map_forcesunlight
	
        nomipmaps
        nopicmip
	
	deformVertexes wave 256 sin 0 1 0.25 0.1
        {
		map models/mapobjects/coroner/tree1a_branch.tga
		alphaFunc GE128
		rgbGen vertex
	}
}

models/mapobjects/coroner/tree1a_branch2
{
	qer_alphafunc greater 0.5
	
	cull disable
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm alphashadow
	surfaceparm nolightmap
	
	//q3map_forcesunlight
	
        nomipmaps
        nopicmip
	
	deformVertexes wave 256 sin 0 1 0.25 0.12
        {
		map models/mapobjects/coroner/tree1a_branch2.tga
		alphaFunc GE128
		rgbGen vertex
	}
}

models/mapobjects/coroner/tree1a_fill
{
	qer_alphafunc greater 0.5
	
	cull disable
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm alphashadow
	surfaceparm nolightmap
	
	//q3map_forcesunlight
	
        nomipmaps
        nopicmip
	
	deformVertexes wave 256 sin 0 1 0.25 0.11
        {
		map models/mapobjects/coroner/tree1a_fill.tga
		alphaFunc GE128
		rgbGen vertex
	}
}

models/mapobjects/coroner/tree1a_radial
{	
	qer_alphafunc greater 0.5
	
	cull disable
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm alphashadow
	surfaceparm nolightmap
	
	//q3map_forcesunlight
	
        nomipmaps
        nopicmip
	
	deformVertexes wave 256 sin 0 1 0.25 0.11
        {
		map models/mapobjects/coroner/tree1a_radial.tga
		alphaFunc GE128
		rgbGen vertex
		//rgbGen exactVertex
	}
}
