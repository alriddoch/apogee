//----------------------------------------------------------------------------//
// model.h                                                                    //
// Copyright (C) 2001 Bruno 'Beosil' Heidelberger                             //
// Copyright (C) 2001-2003 Alistair Riddoch                                   //
//----------------------------------------------------------------------------//
// This program is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU General Public License as published by the Free //
// Software Foundation; either version 2 of the License, or (at your option)  //
// any later version.                                                         //
//----------------------------------------------------------------------------//

#ifndef MODEL_H
#define MODEL_H

//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

#include "GL.h"

#include <cal3d/cal3d.h>

#include <sigc++/signal.h>

#include <set>

//----------------------------------------------------------------------------//
// Class declaration                                                          //
//----------------------------------------------------------------------------//

class Cal3dModel
{
// misc
public:
  static const int STATE_IDLE;
  static const int STATE_FANCY;
  static const int STATE_MOTION;

// member variables
protected:
  int m_state;
  CalCoreModel m_calCoreModel;
  CalModel * m_calModel;
  int m_animationId[16];
  int m_animationCount;
  int m_meshId[32];
  int m_meshCount;
  GLuint m_textureId[32];
  int m_textureCount;
  float m_motionBlend[3];
  float m_renderScale;
  float m_lodLevel;
  std::set<int> m_enabledMeshes;

// constructors/destructor
public:
	Cal3dModel();
	virtual ~Cal3dModel();

// member functions
public:
  void executeAction(int action);
  float getLodLevel();
  void getMotionBlend(float *pMotionBlend);
  float getRenderScale();
  int getState();
  bool onInit(const std::string& strFilename);
  void onRender();
  void onSelect();
  void onShutdown();
  void onUpdate(float elapsedSeconds);
  void setLodLevel(float lodLevel);
  void setMotionBlend(float *pMotionBlend, float delay);
  void setState(int state, float delay);

  CalCoreModel & getCoreModel() {
    return m_calCoreModel;
  }

  CalModel & getCalModel() {
    return *m_calModel;
  }

  const std::set<int> & enabledMeshes() const {
    return m_enabledMeshes;
  }

  std::set<int> & enabledMeshes() {
    return m_enabledMeshes;
  }

  sigc::signal<void, const std::string &> skeletonLoaded;
  sigc::signal<void, const std::string &, int> animationLoaded;
  sigc::signal<void, const std::string &, int> actionLoaded;
  sigc::signal<void, const std::string &, int> meshLoaded;
  sigc::signal<void, const std::string &, int> materialLoaded;
protected:
  GLuint loadTexture(const std::string& strFilename);
  void selectMesh();
  void renderMesh(bool bWireframe, bool bLight);
  void renderSkeleton();
  void renderBoundingBox();
};

#endif // VISUAL_MODEL_H

//----------------------------------------------------------------------------//
