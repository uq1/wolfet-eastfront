textures/alpha/fence_m02
{
    surfaceparm metalsteps
    surfaceparm nomarks
    nomipmaps
    nopicmip
    cull disable
    {
        map textures/alpha/fence_m02.tga
        alphaFunc GE128
        depthWrite
        rgbGen vertex
    }
}

textures/alpha/fence_c06
{
    surfaceparm metalsteps
    nomipmaps
    nopicmip
    cull disable
    {
        map textures/alpha/fence_c06.tga
        alphaFunc GE128
        depthWrite
        rgbGen vertex
    }
}

textures/alpha/mesh_c03
{
    surfaceparm grasssteps
    nomipmaps
    cull twosided
    surfaceparm alphashadow
    {
        map textures/alpha/mesh_c03.tga
        alphaFunc GE128
        depthWrite
        rgbGen identity
    }
}
