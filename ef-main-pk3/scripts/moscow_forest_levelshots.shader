 levelshots/moscow_forest_cc_automap
{
    nopicmip
    nocompress
    nomipmaps
    {
        clampmap levelshots/moscow_forest_cc.tga
        depthFunc equal
        rgbGen identity
    }
}
levelshots/moscow_forest_cc_trans
{
    nopicmip
    nocompress
    nomipmaps
    {
         clampmap levelshots/moscow_forest_cc.tga
         blendfunc blend
         rgbGen identity
         alphaGen vertex
    }
}