// ydnar: haloed style 
stealth
{ 
     cull none 
     deformVertexes wave 1 sin -0.5 0 0 1
     noPicmip 
     surfaceparm trans 
     { 
          map textures/sfx/stealth.tga 
          blendFunc GL_SRC_ALPHA GL_ONE 
          rgbGen const ( 0.25 0.25 0.25 )
          tcGen environment 
          tcMod scroll 0.025 -0.07625 
     } 
}


