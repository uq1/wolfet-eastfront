///////////////////////
///////////////////////
// by mlyn3k
///////////////////////
///////////////////////


////////////////////////////////////////////////////
// GLEBY ==========================================
///////////////////////////////////////////////////

textures/exodus_gleba/czarna
{
    q3map_nonplanar
    q3map_shadeangle 120
    //q3map_tcmodscale 1.5 1.5
    qer_editorimage textures/rubble/debri_m01.jpg
    surfaceparm landmine
    surfaceparm gravelsteps
    
    {
        map $lightmap
        rgbGen identity
    }
    {
		map textures/rubble/debri_m01.jpg
        blendFunc filter
    }
    
}

textures/exodus_gleba/szara
{
    q3map_nonplanar
    q3map_shadeangle 120
    //q3map_tcmodscale 1.5 1.5
    qer_editorimage textures/rubble/debri_m05.jpg
    surfaceparm landmine
    surfaceparm gravelsteps
    
    {
        map $lightmap
        rgbGen identity
    }
    {
		map textures/rubble/debri_m05.jpg
        blendFunc filter
    }
    
}

textures/exodus_gleba/czerwona
{
    q3map_nonplanar
    q3map_shadeangle 120
    //q3map_tcmodscale 1.5 1.5
    qer_editorimage textures/exodus/rock002.jpg
    surfaceparm landmine
    surfaceparm gravelsteps
    
    {
        map $lightmap
        rgbGen identity
    }
    {
		map textures/exodus/rock002.jpg
        blendFunc filter
    }
    
}

///////////////////////////////////////////////////////////
// BLENDING ===============================================
///////////////////////////////////////////////////////////

//textures/exodus_gleba/szary_asfalt        // Normal texture blending
//{
//	qer_editorimage textures/exodus/sza.jpg
//  
//    q3map_nonplanar
//    q3map_shadeAngle 120
//   
//    {
//        map textures/exodus/B_sz.jpg    // Primary
//        rgbGen identity
//    }
//    {
//        map textures/exodus/B_as.tga    // Secondary
//        blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//        alphaFunc GE128
//        rgbGen identity
//        alphaGen vertex
//    }
 //   {
//        map $lightmap
 //       blendFunc GL_DST_COLOR GL_ZERO
  //      rgbGen identity
//    }
//}

//////////////////////////////////////////////////////////////
// ALPHY =====================================================
//////////////////////////////////////////////////////////////

textures/exodus_gleba/alpha_000    // Primary texture ONLY
{
    qer_editorimage textures/exodus/alpha_000.tga
    q3map_alphaMod volume
    q3map_alphaMod set 0
    surfaceparm nodraw
    surfaceparm nonsolid
    surfaceparm trans
    qer_trans 0.75
}

textures/exodus_gleba/alpha_025
{
	qer_editorimage textures/exodus/alpha_025.tga
    q3map_alphaMod volume
    q3map_alphaMod set 0.25
    surfaceparm nodraw
    surfaceparm nonsolid
    surfaceparm trans
    qer_trans 0.75
}

textures/exodus_gleba/alpha_050    // Perfect mix of both Primary + Secondary
{
 	qer_editorimage textures/exodus/alpha_050.tga
    q3map_alphaMod volume
    q3map_alphaMod set 0.50
    surfaceparm nodraw
    surfaceparm nonsolid
    surfaceparm trans
    qer_trans 0.75
}

textures/exodus_gleba/alpha_075
{
	qer_editorimage textures/exodus/alpha_075.tga
    q3map_alphaMod volume
    q3map_alphaMod set 0.75
    surfaceparm nodraw
    surfaceparm nonsolid
    surfaceparm trans
    qer_trans 0.75
}

textures/exodus_gleba/alpha_085
{
   qer_editorimage textures/exodus/alpha_085.tga
    q3map_alphaMod volume
    q3map_alphaMod set 0.85
    surfaceparm nodraw
    surfaceparm nonsolid
    surfaceparm trans
   qer_trans 0.75
}

textures/exodus_gleba/alpha_100    // Secondary texture ONLY
{

   qer_editorimage textures/exodus/alpha_100.tga
    q3map_alphaMod volume
   q3map_alphaMod set 1.0
    surfaceparm nodraw
    surfaceparm nonsolid
    surfaceparm trans
    qer_trans 0.75
} 

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
///////  DOTY ////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

//textures/exodus_gleba/CZERWONA_dot
//{
//   qer_editorimage textures/exodus/dcr.jpg
 //  q3map_nonplanar
//   q3map_shadeAngle 120
 //  q3map_tcGen ivector ( 256 0 0 ) ( 0 256 0 )
 //  q3map_alphaMod dotproduct2 ( 0.0 0.0 0.75 )
 //  surfaceparm gravelsteps
 //  surfaceparm landmine

 //  {
//      map textures/exodus/rock002.jpg	// Primary texture
//      rgbGen identity
//   }
//   {
//      map textures/exodus/B_czr.tga	// Secondary
//      blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//      alphaFunc GE128
//      rgbGen identity
//      alphaGen vertex
//   }
//   {
//      map $lightmap
 //     blendFunc GL_DST_COLOR GL_ZERO
//      rgbGen identity
 //  }
//}

////////////////////////////////////////////////
////////////////////////////////////////////////

textures/exodus_gleba/czerw0szaR
{
   qer_editorimage textures/exodus/crsz.jpg
   q3map_nonplanar
   q3map_shadeAngle 120
   q3map_tcGen ivector ( 256 0 0 ) ( 0 256 0 )
   q3map_alphaMod dotproduct2 ( 0.0 0.0 0.75 )
   surfaceparm gravelsteps
   surfaceparm landmine

   {
      map textures/exodus/rock002.jpg	// Primary texture
      rgbGen identity
   }
   {
      map textures/exodus/B_szr.tga	// Secondary
      blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
      alphaFunc GE128
      rgbGen identity
      alphaGen vertex
   }
   {
      map $lightmap
      blendFunc GL_DST_COLOR GL_ZERO
      rgbGen identity
   }
}