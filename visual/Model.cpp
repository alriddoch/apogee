//----------------------------------------------------------------------------//
// Model.cpp                                                                  //
// Copyright (C) 2001 Bruno 'Beosil' Heidelberger                             //
//----------------------------------------------------------------------------//
// This program is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU General Public License as published by the Free //
// Software Foundation; either version 2 of the License, or (at your option)  //
// any later version.                                                         //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

#include "Model.h"

//----------------------------------------------------------------------------//
// Static member variables initialization                                     //
//----------------------------------------------------------------------------//

const int Model::STATE_IDLE = 0;
const int Model::STATE_FANCY = 1;
const int Model::STATE_MOTION = 2;

//----------------------------------------------------------------------------//
// Constructors                                                               //
//----------------------------------------------------------------------------//

Model::Model()
{
  m_state = STATE_IDLE;
  m_motionBlend[0] = 0.6f;
  m_motionBlend[1] = 0.1f;
  m_motionBlend[2] = 0.3f;
  m_animationCount = 0;
  m_meshCount = 0;
  m_renderScale = 1.0f;
  m_renderOffset = 0.0f;
}

//----------------------------------------------------------------------------//
// Destructor                                                                 //
//----------------------------------------------------------------------------//

Model::~Model()
{
}

//----------------------------------------------------------------------------//
// Execute an action of the model                                             //
//----------------------------------------------------------------------------//

void Model::executeAction(int action)
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
// Get the motion blend factors state of the model                            //
//----------------------------------------------------------------------------//

void Model::getMotionBlend(float *pMotionBlend)
{
  pMotionBlend[0] = m_motionBlend[0];
  pMotionBlend[1] = m_motionBlend[1];
  pMotionBlend[2] = m_motionBlend[2];
}

//----------------------------------------------------------------------------//
// Get the render offset of the model                                         //
//----------------------------------------------------------------------------//

float Model::getRenderOffset()
{
  return m_renderOffset;
}

//----------------------------------------------------------------------------//
// Get the render scale of the model                                          //
//----------------------------------------------------------------------------//

float Model::getRenderScale()
{
  return m_renderScale;
}

//----------------------------------------------------------------------------//
// Get the animation state of the model                                       //
//----------------------------------------------------------------------------//

int Model::getState()
{
  return m_state;
}

//----------------------------------------------------------------------------//
// Initialize the model                                                       //
//----------------------------------------------------------------------------//

bool Model::onInit(const std::string& strFilename)
{
  // create the core model instance
  if(!m_calCoreModel.create("dummy"))
  {
    CalError::printLastError();
    return false;
  }

  // open model description file
  std::ifstream file;
  file.open(strFilename.c_str());
  if(!file)
  {
    std::cerr << "Model description file '" << strFilename << "' not found!" << std::endl;
    return false;
  }

  // load projection data
  file >> m_renderScale;
  file >> m_renderOffset;

  // load the core model description
  std::string strSkeletonFilename;
  file >> strSkeletonFilename;
  strSkeletonFilename = Datapath() + strSkeletonFilename;

  if(!m_calCoreModel.loadCoreSkeleton(strSkeletonFilename))
  {
    CalError::printLastError();
    return false;
  }

  // load all core animation descriptions
  std::string strAnimationFilename;
  file >> m_animationCount;

  int animationId;
  for(animationId = 0; animationId < m_animationCount; animationId++)
  {
    file >> strAnimationFilename;
    strAnimationFilename = Datapath() + strAnimationFilename;

    m_animationId[animationId] = m_calCoreModel.loadCoreAnimation(strAnimationFilename);
    if(m_animationId[animationId] == -1)
    {
      CalError::printLastError();
      return false;
    }
  }

  // load all core mesh descriptions
  std::string strMeshFilename;
  file >> m_meshCount;

  int meshId;
  for(meshId = 0; meshId < m_meshCount; meshId++)
  {
    file >> strMeshFilename;
    strMeshFilename = Datapath() + strMeshFilename;

    m_meshId[meshId] = m_calCoreModel.loadCoreMesh(strMeshFilename);
    if(m_meshId[meshId] == -1)
    {
      CalError::printLastError();
      return false;
    }
  }

  // load all model textures
  std::string strTextureFilename;
  file >> m_textureCount;

  int textureId;
  for(textureId = 0; textureId < m_textureCount; textureId++)
  {
    file >> strTextureFilename;
    strTextureFilename = Datapath() + strTextureFilename;
    int width, height;
    file >> width >> height;

    // open texture file
    std::ifstream textureFile;
    textureFile.open(strTextureFilename.c_str(), std::ios::in | std::ios::binary);
    if(!textureFile)
    {
      std::cerr << "Texture file (" << strTextureFilename << " not found!" << std::endl;
      return false;
    }

    // load texture
    unsigned char buffer[512 * 512 * 3];
    textureFile.read((char *)buffer, width * height * 3);
    textureFile.close();

    // generate texture
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &m_textureId[textureId]);
    glBindTexture(GL_TEXTURE_2D, m_textureId[textureId]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
  }

  // fill unused texture slots with the first texture
  for(; textureId < 32; textureId++)
  {
    m_textureId[textureId] = m_textureId[0];
  }

  // close model description file
  file.close();

  // create the model instance from the loaded core model
  if(!m_calModel.create(&m_calCoreModel))
  {
    CalError::printLastError();
    return false;
  }

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

void Model::renderSkeleton()
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
// Render the mesh of the model                                               //
//----------------------------------------------------------------------------//

void Model::renderMesh(bool bWireframe, bool bLight)
{
  // light attributes
  const GLfloat light_ambient[]  = { 0.3f, 0.3f, 0.3f, 1.0f };
  const GLfloat light_diffuse[]  = { 0.8f, 0.8f, 0.8f, 1.0f };
  const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

  // configure light if needed
  if(bLight)
  {
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnableClientState(GL_NORMAL_ARRAY);
  }

  // we will use vertex arrays, so enable it
  glEnableClientState(GL_VERTEX_ARRAY);

  // loop thru all meshes of the model and render them
  int meshId;
  for(meshId = 0; meshId < m_meshCount; meshId++)
  {
    // get the current core mesh
    CalCoreMesh *pCoreMesh;
    pCoreMesh = m_calModel.getCoreModel()->getCoreMesh(m_meshId[meshId]);

    // get transformed vertices of the mesh
    float meshVertex[20000][3];
    m_calModel.getMeshVertices(m_meshId[meshId], &meshVertex[0][0]);

    // get transformed normals of the mesh if needed
    float meshNormal[20000][3];
    if(bLight)
    {
      m_calModel.getMeshNormals(m_meshId[meshId], &meshNormal[0][0]);
    }

    // get the submesh vector and count
    std::vector<CalCoreMesh::SubMesh>& vectorSubMesh = pCoreMesh->getVectorSubMesh();

    int subMeshCount;
    subMeshCount = vectorSubMesh.size();

    // loop thru all submeshes and render them
    int subMeshId;
    for(subMeshId = 0; subMeshId < subMeshCount; subMeshId++)
    {
      CalCoreMesh::SubMesh& subMesh = vectorSubMesh[subMeshId];

      // set the material of this submesh
      if(bLight)
      {
        GLfloat material_ambient[4];
        material_ambient[0] = subMesh.material.ambientColor.red / 255.0f;
        material_ambient[1] = subMesh.material.ambientColor.green / 255.0f;
        material_ambient[2] = subMesh.material.ambientColor.blue / 255.0f;
        material_ambient[3] = subMesh.material.ambientColor.alpha / 255.0f;
        glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);

        GLfloat material_diffuse[4];;
        material_diffuse[0] = subMesh.material.diffuseColor.red / 255.0f;
        material_diffuse[1] = subMesh.material.diffuseColor.green / 255.0f;
        material_diffuse[2] = subMesh.material.diffuseColor.blue / 255.0f;
        material_diffuse[3] = subMesh.material.diffuseColor.alpha / 255.0f;
        glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);

        GLfloat material_specular[4];
        material_specular[0] = subMesh.material.specularColor.red / 255.0f;
        material_specular[1] = subMesh.material.specularColor.green / 255.0f;
        material_specular[2] = subMesh.material.specularColor.blue / 255.0f;
        material_specular[3] = subMesh.material.specularColor.alpha / 255.0f;
        glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);

        GLfloat material_shininess[1];
        material_shininess[0] = 50.0f; // 1.0f / subMesh.material.shininess;
        glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);
      }
      else
      {
        glColor3f(subMesh.material.diffuseColor.red / 255.0f, subMesh.material.diffuseColor.green / 255.0f, subMesh.material.diffuseColor.blue / 255.0f);
      }

      // check if we have a textured submesh
      bool bTexture;
      bTexture = !subMesh.material.vectorMap.empty();

      if(bTexture)
      {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m_textureId[subMesh.material.vectorMap[0].id]);
        glEnable(GL_COLOR_MATERIAL);
        glColor3f(1.0f, 1.0f, 1.0f);
      }

      // fill the different arrays for vertex, normal and texture coordinate data
      static float renderVertex[10000][3];
      static float renderNormal[10000][3];
      static float renderTextureCoordinate[10000][2];

      int vertexCount;
      vertexCount = subMesh.vectorVertexId.size();

      int vertexId;
      for(vertexId = 0; vertexId < vertexCount; vertexId++)
      {
        // fill in vertex data
        renderVertex[vertexId][0] = meshVertex[subMesh.vectorVertexId[vertexId]][0];
        renderVertex[vertexId][1] = meshVertex[subMesh.vectorVertexId[vertexId]][1];
        renderVertex[vertexId][2] = meshVertex[subMesh.vectorVertexId[vertexId]][2];

        // fill in normal data if needed
        if(bLight)
        {
          renderNormal[vertexId][0] = meshNormal[subMesh.vectorVertexId[vertexId]][0];
          renderNormal[vertexId][1] = meshNormal[subMesh.vectorVertexId[vertexId]][1];
          renderNormal[vertexId][2] = meshNormal[subMesh.vectorVertexId[vertexId]][2];
        }

        // fill in texture coordinate data if needed
        if(bTexture)
        {
          renderTextureCoordinate[vertexId][0] = subMesh.vectorvectorTextureCoordinate[0][vertexId].u;
          renderTextureCoordinate[vertexId][1] = 1.0f - subMesh.vectorvectorTextureCoordinate[0][vertexId].v;
        }
      }

      // fill the face/index array with data
      static unsigned int renderFace[20000];

      int faceCount;
      faceCount = subMesh.vectorFace.size();

      int faceId;
      for(faceId = 0; faceId < faceCount; faceId++)
      {
        // fill in face data
        renderFace[faceId * 3] = subMesh.vectorFace[faceId].vertexId[0];
        renderFace[faceId * 3 + 1] = subMesh.vectorFace[faceId].vertexId[1];
        renderFace[faceId * 3 + 2] = subMesh.vectorFace[faceId].vertexId[2];
      }

      // set all array pointers
      glVertexPointer(3, GL_FLOAT, 0, &renderVertex[0][0]);

      if(bLight)
      {
        glNormalPointer(GL_FLOAT, 0, &renderNormal[0][0]);
      }

      if(bTexture)
      {
        glTexCoordPointer(2, GL_FLOAT, 0, &renderTextureCoordinate[0][0]);
      }

      // draw the vertex array (the wireframe mode is implemented very dirty btw. =8] )
      glDrawElements(bWireframe ? GL_LINES : GL_TRIANGLES, faceCount * 3, GL_UNSIGNED_INT, &renderFace[0]);

      // clear texture states
      if(bTexture)
      {
        glDisable(GL_COLOR_MATERIAL);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisable(GL_TEXTURE_2D);
      }
    }
  }

  // clear vertex array state
  glDisableClientState(GL_VERTEX_ARRAY);

  // clear light state
  if(bLight)
  {
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
  }
}

//----------------------------------------------------------------------------//
// Render the model                                                           //
//----------------------------------------------------------------------------//

void Model::onRender()
{
  // set global OpenGL states
  // glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);

  renderMesh(false, true);

  // clear global OpenGL states
  // glDisable(GL_DEPTH_TEST);
}

//----------------------------------------------------------------------------//
// Update the model                                                           //
//----------------------------------------------------------------------------//

void Model::onUpdate(float elapsedSeconds)
{
  // update the model
  m_calModel.update(elapsedSeconds);
}

//----------------------------------------------------------------------------//
// Shut the model down                                                        //
//----------------------------------------------------------------------------//

void Model::onShutdown()
{
  // destroy the model instance
  m_calModel.destroy();

  // destroy the core model instance
  m_calCoreModel.destroy();
}

//----------------------------------------------------------------------------//
// Set the motion blend factors state of the model                            //
//----------------------------------------------------------------------------//

void Model::setMotionBlend(float *pMotionBlend, float delay)
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

void Model::setState(int state, float delay)
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
