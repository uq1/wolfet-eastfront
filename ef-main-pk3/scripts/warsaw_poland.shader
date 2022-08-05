 levelshots/warsaw_poland_cc_automap
{
    nopicmip
    nocompress
    nomipmaps
    {
        clampmap levelshots/warsaw_poland_cc.tga
        depthFunc equal
        rgbGen identity
    }
}

 levelshots/warsaw_poland_cc_trans
{
    nopicmip
    nocompress
    nomipmaps
    {
        clampmap levelshots/warsaw_poland_cc.tga
        blendfunc blend
        rgbGen identity
        alphaGen vertex
    }
}