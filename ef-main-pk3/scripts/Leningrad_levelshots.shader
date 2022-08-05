 levelshots/leningrad_cc_automap
{
    nopicmip
    nocompress
    nomipmaps
    {
        clampmap levelshots/leningrad_cc.tga
        depthFunc equal
        rgbGen identity
    }
}
levelshots/leningrad_cc_trans
{
    nopicmip
    nocompress
    nomipmaps
    {
         clampmap levelshots/leningrad_cc.tga
         blendfunc blend
         rgbGen identity
         alphaGen vertex
    }
}