//----------------------------------------------------------------------------//
// model.cpp                                                                  //
// Copyright (C) 2001 Bruno 'Beosil' Heidelberger                             //
// Copyright (C) 2001-2003 Alistair Riddoch                                   //
//----------------------------------------------------------------------------//
// This program is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU General Public License as published by the Free //
// Software Foundation; either version 2 of the License, or (at your option)  //
// any later version.                                                         //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

#include "GL.h"

#include "Model.h"

#include "Texture.h"

#include <cal3d/platform.h>

//----------------------------------------------------------------------------//
// Static member variables initialization                                     //
//----------------------------------------------------------------------------//

const int Cal3dModel::STATE_IDLE = 0;
const int Cal3dModel::STATE_FANCY = 1;
const int Cal3dModel::STATE_MOTION = 2;

//----------------------------------------------------------------------------//
// Constructors                                                               //
//----------------------------------------------------------------------------//

Cal3dModel::Cal3dModel()
{
  m_state = STATE_IDLE;
  m_motionBlend[0] = 0.6f;
  m_motionBlend[1] = 0.1f;
  m_motionBlend[2] = 0.3f;
  m_animationCount = 0;
  m_meshCount = 0;
  m_renderScale = 1.0f;
  m_lodLevel = 1.0f;
}

//----------------------------------------------------------------------------//
// Destructor                                                                 //
//----------------------------------------------------------------------------//

Cal3dModel::~Cal3dModel()
{
}

//----------------------------------------------------------------------------//
// Execute an action of the model                                             //
//----------------------------------------------------------------------------//

void Cal3dModel::executeAction(int action)
{
  switch(action)
  {
    case 0:
      m_calModel.getMixer()->executeAction(m_animationId[5], 0.3f, 0.3f);
      break;
    case 1:
      m_calModel.getMixer()->executeAction(m_animationId[6], 0.3f, 0.3f);
      break;
  }
}

//----------------------------------------------------------------------------//
// Get the lod level of the model                                             //
//----------------------------------------------------------------------------//

float Cal3dModel::getLodLevel()
{
  return m_lodLevel;
}

//----------------------------------------------------------------------------//
// Get the motion blend factors state of the model                            //
//----------------------------------------------------------------------------//

void Cal3dModel::getMotionBlend(float *pMotionBlend)
{
  pMotionBlend[0] = m_motionBlend[0];
  pMotionBlend[1] = m_motionBlend[1];
  pMotionBlend[2] = m_motionBlend[2];
}

//----------------------------------------------------------------------------//
// Get the render scale of the model                                          //
//----------------------------------------------------------------------------//

float Cal3dModel::getRenderScale()
{
  return m_renderScale;
}

//----------------------------------------------------------------------------//
// Get the animation state of the model                                       //
//----------------------------------------------------------------------------//

int Cal3dModel::getState()
{
  return m_state;
}

//----------------------------------------------------------------------------//
// Load and create a texture from a given file                                //
//----------------------------------------------------------------------------//

GLuint Cal3dModel::loadTexture(const std::string& strFilename)
{
  GLuint texture = Texture::get(strFilename, false, GL_LINEAR_MIPMAP_NEAREST);
  if (texture != Texture::getDefault()) {
    return texture;
  }

  // open the texture file
  std::ifstream file;
  file.open(strFilename.c_str(), std::ios::in | std::ios::binary);
  if(!file)
  {
    std::cerr << "Texture file '" << strFilename << "' not found." << std::endl;
    return texture;
  }

  // load the dimension of the texture
  int width;
  CalPlatform::readInteger(file, width);
  int height;
  CalPlatform::readInteger(file, height);
  int depth;
  CalPlatform::readInteger(file, depth);

  // allocate a temporary buffer to load the texture to
  unsigned char *pBuffer;
  pBuffer = new unsigned char[2 * width * height * depth];
  if(pBuffer == 0)
  {
    std::cerr << "Memory allocation for texture '" << strFilename << "' failed." << std::endl;
    return texture;
  }

  // load the texture
  file.read((char *)pBuffer, width * height * depth);

  // explicitely close the file
  file.close();

  // flip texture around y-axis (-> opengl-style)
  int y;
  for(y = 0; y < height; y++)
  {
    memcpy(&pBuffer[(height + y) * width * depth], &pBuffer[(height - y - 1) * width * depth], width * depth);
  }

  // generate texture
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  GLuint textureId;
  glGenTextures(1, &textureId);
  glBindTexture(GL_TEXTURE_2D, textureId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, (depth == 3) ? GL_RGB : GL_RGBA, width, height, 0, (depth == 3) ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, &pBuffer[width * height * depth]);

  // free the allocated memory
  delete [] pBuffer;

  return textureId;
}

//----------------------------------------------------------------------------//
// Initialize the model                                                       //
//----------------------------------------------------------------------------//

bool Cal3dModel::onInit(const std::string& strFilename)
{
  // open the model configuration file
  std::ifstream file;
  file.open(strFilename.c_str(), std::ios::in | std::ios::binary);
  if(!file)
  {
    std::cerr << "Failed to open model configuration file '" << strFilename << "'." << std::endl;
    return false;
  }

  // create a core model instance
  if(!m_calCoreModel.create("dummy"))
  {
    CalError::printLastError();
    return false;
  }

  // initialize the data path
  std::string strPath;
  unsigned int pos = strFilename.find_last_of("/");
  if (pos != std::string::npos) {
    strPath = strFilename.substr(0, pos + 1);
  }
  std::cout << "Got directory path \"" << strPath << "\" from filename \""
            << strFilename << "\"" << std::endl << std::flush;

  // initialize the animation count
  int animationCount;
  animationCount = 0;

  // parse all lines from the model configuration file
  int line;
  for(line = 1; ; line++)
  {
    // read the next model configuration line
    std::string strBuffer;
    std::getline(file, strBuffer);

    // stop if we reached the end of file
    if(file.eof()) break;

    // check if an error happend while reading from the file
    if(!file)
    {
      std::cerr << "Error while reading from the model configuration file '" << strFilename << "'." << std::endl;
      return false;
    }

    // find the first non-whitespace character
    std::string::size_type pos;
    pos = strBuffer.find_first_not_of(" \t");

    // check for empty lines
    if((pos == std::string::npos) || (strBuffer[pos] == '\n') || (strBuffer[pos] == '\r') || (strBuffer[pos] == 0)) continue;

    // check for comment lines
    if(strBuffer[pos] == '#') continue;

    // get the key
    std::string strKey;
    strKey = strBuffer.substr(pos, strBuffer.find_first_of(" =\t\n\r", pos) - pos);
    pos += strKey.size();

    // get the '=' character
    pos = strBuffer.find_first_not_of(" \t", pos);
    if((pos == std::string::npos) || (strBuffer[pos] != '='))
    {
      std::cerr << strFilename << "(" << line << "): Invalid syntax." << std::endl;
      return false;
    }

    // find the first non-whitespace character after the '=' character
    pos = strBuffer.find_first_not_of(" \t", pos + 1);

    // get the data
    std::string strData;
    strData = strBuffer.substr(pos, strBuffer.find_first_of("\n\r", pos) - pos);

    // handle the model creation
    if(strKey == "scale")
    {
      // set rendering scale factor
      m_renderScale = atof(strData.c_str());
    }
    else if(strKey == "path")
    {
      // set the new path for the data files
      strPath = strData;
    }
    else if(strKey == "skeleton")
    {
      // load core skeleton
      std::cout << "Loading skeleton '" << strData << "'..." << std::endl;
      if(!m_calCoreModel.loadCoreSkeleton(strPath + strData))
      {
        CalError::printLastError();
        return false;
      }
      skeletonLoaded.emit(strData);
    }
    else if(strKey == "animation")
    {
      // load core animation
      std::cout << "Loading animation '" << strData << "'..." << std::endl;
      int animationId = m_calCoreModel.loadCoreAnimation(strPath + strData);
      if(animationId == -1)
      {
        CalError::printLastError();
        return false;
      }
      m_animationId[animationCount] = animationId;
      animationLoaded.emit(strData, animationId);
      animationCount++;
    }
    else if(strKey == "animation_action")
    {
      // FIXME - load an animation action.
      // Probably the cally demo can help us out here.
    }
    else if(strKey == "mesh")
    {
      // load core mesh
      std::cout << "Loading mesh '" << strData << "'..." << std::endl;
      int meshId = m_calCoreModel.loadCoreMesh(strPath + strData);
      if(meshId == -1)
      {
        CalError::printLastError();
        return false;
      }
      meshLoaded.emit(strData, meshId);
    }
    else if(strKey == "material")
    {
      // load core material
      std::string strFullPath = strPath + strData;
      std::cout << "Loading material '" << strData << "'..." << std::endl;
      int materialId;
      if((materialId = m_calCoreModel.loadCoreMaterial(strPath + strData))
                                                                        == -1)
      {
        CalError::printLastError();
        return false;
      }
      materialLoaded.emit(strData, materialId);
      CalCoreMaterial *pCoreMaterial = m_calCoreModel.getCoreMaterial(materialId);
      std::string strMatPath;
      unsigned int pos = strFullPath.find_last_of("/");
      if (pos != std::string::npos) {
        strMatPath = strFullPath.substr(0, pos + 1);
      }
      // loop through all maps of the core material
      int mapId;
      for(mapId = 0; mapId < pCoreMaterial->getMapCount(); mapId++)
      {
        // get the filename of the texture
        std::string strFilename;
        strFilename = pCoreMaterial->getMapFilename(mapId);
  
        // load the texture from the file
        GLuint textureId;
        textureId = loadTexture(strMatPath + strFilename);
  
        // store the opengl texture id in the user data of the map
        pCoreMaterial->setMapUserData(mapId, (Cal::UserData)textureId);
      }
    }
    else
    {
      std::cerr << strFilename << "(" << line
                << "): Invalid syntax. Unknown key [" << strKey << "]"
                << std::endl;
      return false;
    }
  }

  // explicitely close the file
  file.close();

#if 0
  // Moved this code to the section above, so textures can be loaded from
  // directory relative to the material file that references them.
  // load all textures and store the opengl texture id in the corresponding map in the material
  int materialId;
  for(materialId = 0; materialId < m_calCoreModel.getCoreMaterialCount(); materialId++)
  {
    // get the core material
    CalCoreMaterial *pCoreMaterial;
    pCoreMaterial = m_calCoreModel.getCoreMaterial(materialId);

    // loop through all maps of the core material
    int mapId;
    for(mapId = 0; mapId < pCoreMaterial->getMapCount(); mapId++)
    {
      // get the filename of the texture
      std::string strFilename;
      strFilename = pCoreMaterial->getMapFilename(mapId);

      // load the texture from the file
      GLuint textureId;
      textureId = loadTexture(strPath + strFilename);

      // store the opengl texture id in the user data of the map
      pCoreMaterial->setMapUserData(mapId, (Cal::UserData)textureId);
    }
  }
#endif

  // make one material thread for each material
  // NOTE: this is not the right way to do it, but this viewer can't do the right
  // mapping without further information on the model etc.
  int materialId;
  for(materialId = 0; materialId < m_calCoreModel.getCoreMaterialCount(); materialId++)
  {
    // create the a material thread
    m_calCoreModel.createCoreMaterialThread(materialId);

    // initialize the material thread
    m_calCoreModel.setCoreMaterialId(materialId, 0, materialId);
  }

  // Calculate Bounding Boxes

  m_calCoreModel.getCoreSkeleton()->calculateBoundingBox(&m_calCoreModel);

  // create the model instance from the loaded core model
  if(!m_calModel.create(&m_calCoreModel))
  {
    CalError::printLastError();
    return false;
  }

  // attach all meshes to the model
  int meshId;
  for(meshId = 0; meshId < m_calCoreModel.getCoreMeshCount(); meshId++)
  {
    m_calModel.attachMesh(meshId);
  }

  // set the material set of the whole model
  m_calModel.setMaterialSet(0);

  // set initial animation state
  m_state = STATE_MOTION;
  m_calModel.getMixer()->blendCycle(m_animationId[STATE_MOTION], m_motionBlend[0], 0.0f);
  m_calModel.getMixer()->blendCycle(m_animationId[STATE_MOTION + 1], m_motionBlend[1], 0.0f);
  m_calModel.getMixer()->blendCycle(m_animationId[STATE_MOTION + 2], m_motionBlend[2], 0.0f);

  return true;
}

//----------------------------------------------------------------------------//
// Render the skeleton of the model                                           //
//----------------------------------------------------------------------------//

void Cal3dModel::renderSkeleton()
{
  // draw the bone lines
  float lines[1024][2][3];
  int nrLines;
  nrLines =  m_calModel.getSkeleton()->getBoneLines(&lines[0][0][0]);
//  nrLines = m_calModel.getSkeleton()->getBoneLinesStatic(&lines[0][0][0]);

  glLineWidth(3.0f);
  glColor3f(1.0f, 1.0f, 1.0f);
  glBegin(GL_LINES);
    int currLine;
    for(currLine = 0; currLine < nrLines; currLine++)
    {
      glVertex3f(lines[currLine][0][0], lines[currLine][0][1], lines[currLine][0][2]);
      glVertex3f(lines[currLine][1][0], lines[currLine][1][1], lines[currLine][1][2]);
    }
  glEnd();
  glLineWidth(1.0f);

  // draw the bone points
  float points[1024][3];
  int nrPoints;
  nrPoints =  m_calModel.getSkeleton()->getBonePoints(&points[0][0]);
//  nrPoints = m_calModel.getSkeleton()->getBonePointsStatic(&points[0][0]);

  glPointSize(4.0f);
  glBegin(GL_POINTS);
    glColor3f(0.0f, 0.0f, 1.0f);
    int currPoint;
    for(currPoint = 0; currPoint < nrPoints; currPoint++)
    {
      glVertex3f(points[currPoint][0], points[currPoint][1], points[currPoint][2]);
    }
  glEnd();
  glPointSize(1.0f);
}

//----------------------------------------------------------------------------//
// Render the bounding boxes of a model                                       //
//----------------------------------------------------------------------------//

void Cal3dModel::renderBoundingBox()
{  

   CalSkeleton *pCalSkeleton = m_calModel.getSkeleton();

   pCalSkeleton->calculateBoundingBox();

   std::vector<CalBone*> &vectorCoreBone = pCalSkeleton->getVectorBone();

   glColor3f(1.0f, 1.0f, 1.0f);
   glBegin(GL_LINES);      

   for(size_t boneId=0;boneId<vectorCoreBone.size();++boneId)
   {
      CalBoundingBox & calBoundingBox  = vectorCoreBone[boneId]->getBoundingBox();

	  CalVector p[8];
	  calBoundingBox.computePoints(p);


	  glVertex3f(p[0].x,p[0].y,p[0].z);
	  glVertex3f(p[1].x,p[1].y,p[1].z);

	  glVertex3f(p[0].x,p[0].y,p[0].z);
	  glVertex3f(p[2].x,p[2].y,p[2].z);

	  glVertex3f(p[1].x,p[1].y,p[1].z);
	  glVertex3f(p[3].x,p[3].y,p[3].z);

	  glVertex3f(p[2].x,p[2].y,p[2].z);
	  glVertex3f(p[3].x,p[3].y,p[3].z);

	  glVertex3f(p[4].x,p[4].y,p[4].z);
	  glVertex3f(p[5].x,p[5].y,p[5].z);

	  glVertex3f(p[4].x,p[4].y,p[4].z);
	  glVertex3f(p[6].x,p[6].y,p[6].z);

	  glVertex3f(p[5].x,p[5].y,p[5].z);
	  glVertex3f(p[7].x,p[7].y,p[7].z);

	  glVertex3f(p[6].x,p[6].y,p[6].z);
	  glVertex3f(p[7].x,p[7].y,p[7].z);

	  glVertex3f(p[0].x,p[0].y,p[0].z);
	  glVertex3f(p[4].x,p[4].y,p[4].z);

	  glVertex3f(p[1].x,p[1].y,p[1].z);
	  glVertex3f(p[5].x,p[5].y,p[5].z);

	  glVertex3f(p[2].x,p[2].y,p[2].z);
	  glVertex3f(p[6].x,p[6].y,p[6].z);

	  glVertex3f(p[3].x,p[3].y,p[3].z);
	  glVertex3f(p[7].x,p[7].y,p[7].z);  

   }

   glEnd();

}


//----------------------------------------------------------------------------//
// Render the mesh of the model                                               //
//----------------------------------------------------------------------------//

void Cal3dModel::renderMesh(bool bWireframe, bool bLight)
{
  // get the renderer of the model
  CalRenderer *pCalRenderer;
  pCalRenderer = m_calModel.getRenderer();

  if(pCalRenderer == 0) return;

  // begin the rendering loop
  if(!pCalRenderer->beginRendering()) return;

  glCullFace(GL_FRONT);
  // set wireframe mode if necessary
  if(bWireframe)
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }

  // set the global OpenGL states
  //glEnable(GL_DEPTH_TEST);
  // glShadeModel(GL_SMOOTH);

  // set the lighting mode if necessary
  if(bLight)
  {
    // glEnable(GL_LIGHTING);
    // glEnable(GL_LIGHT0);
  }

  // we will use vertex arrays, so enable them
  glEnableClientState(GL_NORMAL_ARRAY);

  // get the number of meshes
  int meshCount;
  meshCount = pCalRenderer->getMeshCount();

  // render all meshes of the model
  int meshId;
  for(meshId = 0; meshId < meshCount; meshId++)
  {
    // get the number of submeshes
    int submeshCount;
    submeshCount = pCalRenderer->getSubmeshCount(meshId);

    // render all submeshes of the mesh
    int submeshId;
    for(submeshId = 0; submeshId < submeshCount; submeshId++)
    {
      // select mesh and submesh for further data access
      if(pCalRenderer->selectMeshSubmesh(meshId, submeshId))
      {
        unsigned char meshColor[4];
        GLfloat materialColor[4];

        // set the material ambient color
        pCalRenderer->getAmbientColor(&meshColor[0]);
        materialColor[0] = meshColor[0] / 255.0f;  materialColor[1] = meshColor[1] / 255.0f; materialColor[2] = meshColor[2] / 255.0f; materialColor[3] = meshColor[3] / 255.0f;
        glMaterialfv(GL_FRONT, GL_AMBIENT, materialColor);

        // set the material diffuse color
        pCalRenderer->getDiffuseColor(&meshColor[0]);
        materialColor[0] = meshColor[0] / 255.0f;  materialColor[1] = meshColor[1] / 255.0f; materialColor[2] = meshColor[2] / 255.0f; materialColor[3] = meshColor[3] / 255.0f;
        glMaterialfv(GL_FRONT, GL_DIFFUSE, materialColor);

        // set the vertex color if we have no lights
        if(!bLight)
        {
          // glColor4fv(materialColor);
        }

        // set the material specular color
        pCalRenderer->getSpecularColor(&meshColor[0]);
        materialColor[0] = meshColor[0] / 255.0f;  materialColor[1] = meshColor[1] / 255.0f; materialColor[2] = meshColor[2] / 255.0f; materialColor[3] = meshColor[3] / 255.0f;
        glMaterialfv(GL_FRONT, GL_SPECULAR, materialColor);

        // set the material shininess factor
        float shininess;
        shininess = 50.0f; //TODO: pCalRenderer->getShininess();
        glMaterialfv(GL_FRONT, GL_SHININESS, &shininess);

        // get the transformed vertices of the submesh
        static float meshVertices[30000][3];
        int vertexCount;
        vertexCount = pCalRenderer->getVertices(&meshVertices[0][0]);

        // get the transformed normals of the submesh
        static float meshNormals[30000][3];
        pCalRenderer->getNormals(&meshNormals[0][0]);

        // get the texture coordinates of the submesh
        static float meshTextureCoordinates[30000][2];
        int textureCoordinateCount;
        textureCoordinateCount = pCalRenderer->getTextureCoordinates(0, &meshTextureCoordinates[0][0]);

        // get the faces of the submesh
        static CalIndex meshFaces[50000][3];
        int faceCount;
        faceCount = pCalRenderer->getFaces(&meshFaces[0][0]);

        // set the vertex and normal buffers
        glVertexPointer(3, GL_FLOAT, 0, &meshVertices[0][0]);
        glNormalPointer(GL_FLOAT, 0, &meshNormals[0][0]);

        // set the texture coordinate buffer and state if necessary
        if((pCalRenderer->getMapCount() > 0) && (textureCoordinateCount > 0))
        {
          glEnable(GL_TEXTURE_2D);
          glEnableClientState(GL_TEXTURE_COORD_ARRAY);
          glEnable(GL_COLOR_MATERIAL);

          // set the texture id we stored in the map user data
          glBindTexture(GL_TEXTURE_2D, (GLuint)pCalRenderer->getMapUserData(0));

          // set the texture coordinate buffer
          glTexCoordPointer(2, GL_FLOAT, 0, &meshTextureCoordinates[0][0]);
          glColor3f(1.0f, 1.0f, 1.0f);
        }

        // draw the submesh
        
        if(sizeof(CalIndex)==2)
            glDrawElements(GL_TRIANGLES, faceCount * 3, GL_UNSIGNED_SHORT, &meshFaces[0][0]);
        else
            glDrawElements(GL_TRIANGLES, faceCount * 3, GL_UNSIGNED_INT, &meshFaces[0][0]);

        // disable the texture coordinate state if necessary
        if((pCalRenderer->getMapCount() > 0) && (textureCoordinateCount > 0))
        {
          glDisable(GL_COLOR_MATERIAL);
          glDisableClientState(GL_TEXTURE_COORD_ARRAY);
          glDisable(GL_TEXTURE_2D);
        }

// DEBUG-CODE //////////////////////////////////////////////////////////////////
/*
glBegin(GL_LINES);
glColor3f(1.0f, 1.0f, 1.0f);
int vertexId;
for(vertexId = 0; vertexId < vertexCount; vertexId++)
{
const float scale = 0.3f;
  glVertex3f(meshVertices[vertexId][0], meshVertices[vertexId][1], meshVertices[vertexId][2]);
  glVertex3f(meshVertices[vertexId][0] + meshNormals[vertexId][0] * scale, meshVertices[vertexId][1] + meshNormals[vertexId][1] * scale, meshVertices[vertexId][2] + meshNormals[vertexId][2] * scale);
}
glEnd();
*/
////////////////////////////////////////////////////////////////////////////////
      }
    }
  }

  // clear vertex array state
  glDisableClientState(GL_NORMAL_ARRAY);

  // reset the lighting mode
  if(bLight)
  {
    // glDisable(GL_LIGHTING);
    // glDisable(GL_LIGHT0);
  }

  // reset the global OpenGL states
  // glDisable(GL_DEPTH_TEST);

  // reset wireframe mode if necessary
  if(bWireframe)
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
  glCullFace(GL_BACK);

  // end the rendering
  pCalRenderer->endRendering();
}

void Cal3dModel::selectMesh()
{
  // get the renderer of the model
  CalRenderer *pCalRenderer;
  pCalRenderer = m_calModel.getRenderer();

  // begin the rendering loop
  if(!pCalRenderer->beginRendering()) return;

  // we will use vertex arrays, so enable them

  // get the number of meshes
  int meshCount;
  meshCount = pCalRenderer->getMeshCount();

  // render all meshes of the model
  int meshId;
  for(meshId = 0; meshId < meshCount; meshId++)
  {
    // get the number of submeshes
    int submeshCount;
    submeshCount = pCalRenderer->getSubmeshCount(meshId);

    // render all submeshes of the mesh
    int submeshId;
    for(submeshId = 0; submeshId < submeshCount; submeshId++)
    {
      // select mesh and submesh for further data access
      if(pCalRenderer->selectMeshSubmesh(meshId, submeshId))
      {
        // get the transformed vertices of the submesh
        static float meshVertices[30000][3];
        int vertexCount;
        vertexCount = pCalRenderer->getVertices(&meshVertices[0][0]);

        // get the faces of the submesh
        static int meshFaces[50000][3];
        int faceCount;
        faceCount = pCalRenderer->getFaces(&meshFaces[0][0]);

        // set the vertex and normal buffers
        glVertexPointer(3, GL_FLOAT, 0, &meshVertices[0][0]);

        // draw the submesh
        glDrawElements(GL_TRIANGLES, faceCount * 3, GL_UNSIGNED_INT, &meshFaces[0][0]);

      }
    }
  }

  // clear vertex array state

  // end the rendering
  pCalRenderer->endRendering();
}
//----------------------------------------------------------------------------//
// Render the model                                                           //
//----------------------------------------------------------------------------//

void Cal3dModel::onRender()
{
  // set global OpenGL states
  //glEnable(GL_DEPTH_TEST);
  // glShadeModel(GL_SMOOTH);

  // check if we need to render the skeleton
  //if(theMenu.isSkeleton())
  //{
    //renderSkeleton();
  //}

  // check if we need to render the mesh
  //if(!theMenu.isSkeleton() || theMenu.isWireframe())
  //{
    //renderMesh(theMenu.isWireframe(), theMenu.isLight());
    renderMesh(false, true);
  //}

  // clear global OpenGL states
  // glDisable(GL_DEPTH_TEST);
}

void Cal3dModel::onSelect()
{
    selectMesh();
}
//----------------------------------------------------------------------------//
// Update the model                                                           //
//----------------------------------------------------------------------------//

void Cal3dModel::onUpdate(float elapsedSeconds)
{
  // update the model
  m_calModel.update(elapsedSeconds);
}

//----------------------------------------------------------------------------//
// Shut the model down                                                        //
//----------------------------------------------------------------------------//

void Cal3dModel::onShutdown()
{
  // destroy the model instance
  m_calModel.destroy();

  // destroy the core model instance
  m_calCoreModel.destroy();
}

//----------------------------------------------------------------------------//
// Set the lod level of the model                                             //
//----------------------------------------------------------------------------//

void Cal3dModel::setLodLevel(float lodLevel)
{
  m_lodLevel = lodLevel;

  // set the new lod level in the cal model renderer
  m_calModel.setLodLevel(m_lodLevel);
}

//----------------------------------------------------------------------------//
// Set the motion blend factors state of the model                            //
//----------------------------------------------------------------------------//

void Cal3dModel::setMotionBlend(float *pMotionBlend, float delay)
{
  m_motionBlend[0] = pMotionBlend[0];
  m_motionBlend[1] = pMotionBlend[1];
  m_motionBlend[2] = pMotionBlend[2];

  m_calModel.getMixer()->clearCycle(m_animationId[STATE_IDLE], delay);
  m_calModel.getMixer()->clearCycle(m_animationId[STATE_FANCY], delay);
  m_calModel.getMixer()->blendCycle(m_animationId[STATE_MOTION], m_motionBlend[0], delay);
  m_calModel.getMixer()->blendCycle(m_animationId[STATE_MOTION + 1], m_motionBlend[1], delay);
  m_calModel.getMixer()->blendCycle(m_animationId[STATE_MOTION + 2], m_motionBlend[2], delay);

  m_state = STATE_MOTION;
}

//----------------------------------------------------------------------------//
// Set a new animation state within a given delay                             //
//----------------------------------------------------------------------------//

void Cal3dModel::setState(int state, float delay)
{
  // check if this is really a new state
  if(state != m_state)
  {
    if(state == STATE_IDLE)
    {
      m_calModel.getMixer()->blendCycle(m_animationId[STATE_IDLE], 1.0f, delay);
      m_calModel.getMixer()->clearCycle(m_animationId[STATE_FANCY], delay);
      m_calModel.getMixer()->clearCycle(m_animationId[STATE_MOTION], delay);
      m_calModel.getMixer()->clearCycle(m_animationId[STATE_MOTION + 1], delay);
      m_calModel.getMixer()->clearCycle(m_animationId[STATE_MOTION + 2], delay);
      m_state = STATE_IDLE;
    }
    else if(state == STATE_FANCY)
    {
      m_calModel.getMixer()->clearCycle(m_animationId[STATE_IDLE], delay);
      m_calModel.getMixer()->blendCycle(m_animationId[STATE_FANCY], 1.0f, delay);
      m_calModel.getMixer()->clearCycle(m_animationId[STATE_MOTION], delay);
      m_calModel.getMixer()->clearCycle(m_animationId[STATE_MOTION + 1], delay);
      m_calModel.getMixer()->clearCycle(m_animationId[STATE_MOTION + 2], delay);
      m_state = STATE_FANCY;
    }
    else if(state == STATE_MOTION)
    {
      m_calModel.getMixer()->clearCycle(m_animationId[STATE_IDLE], delay);
      m_calModel.getMixer()->clearCycle(m_animationId[STATE_FANCY], delay);
      m_calModel.getMixer()->blendCycle(m_animationId[STATE_MOTION], m_motionBlend[0], delay);
      m_calModel.getMixer()->blendCycle(m_animationId[STATE_MOTION + 1], m_motionBlend[1], delay);
      m_calModel.getMixer()->blendCycle(m_animationId[STATE_MOTION + 2], m_motionBlend[2], delay);
      m_state = STATE_MOTION;
    }
  }
}

//----------------------------------------------------------------------------//
