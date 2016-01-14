#ifndef ROOTSCENE
#define ROOTSCENE

/* RootScene
 * It contains all the entities that will appear on the scene.
 * It includes both user scene and utility entities (axis).
 * The user scene has the following structure (tree branch example):
 * [Root] -> <Transform1> -> (Canvas11) -> `Stroke111`
 *        -> <Transform2> -> (Canvas21)
 *                        -> (Canvas22) -> `Stroke221`
 *                                      -> `Stroke222`
 * ...
 *       -> {Switch} -> {Axes}
 *
 * Where we denote:
 * [] - an osg::Group inherited
 * <> - osg::MatrixTransform inherited
 * () - osg::Geode inherited
 * `` - osg::Drawable inherited
 * {} - other osg inherited types such as camera or switch nodes
 *
 * When performing addCanvas() or deleteCanvas(), this RootScene
 * tries to delete the canvases directly from _userScene through the
 * API methods.
 */

#include <iostream>
#include <string>
#include <string>

#include <osg/ref_ptr>
#include <osg/observer_ptr>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Node>
#include <osg/Matrix>

#include "axes.h"
#include "Canvas.h"
#include "Photo.h"
#include "Stroke.h"
#include "settings.h"
#include "../libSGUtils/observescenecallback.h"
#include "../libGUI/hudcamera.h"
#include "../libSGControls/AddStrokeCommand.h"

#include <QUndoStack>

class AddStrokeCommand;

class RootScene : public osg::Group {
public:
    RootScene(QUndoStack* undoStack = 0);
    ~RootScene();

    osg::Group* getUserScene() const;

    void setAxesVisibility(bool vis);
    bool getAxesVisibility() const;
    const Axes* getAxes() const;

    void setNameUserScene(const std::string& name);
    std::string getNameUserScene() const;

    const ObserveSceneCallback* getSceneObserver() const;
    const HUDCamera* getHudCamera() const;
    void setHudCameraVisibility(bool vis);
    bool getHudCameraVisibility() const;

    void addStroke(float u, float v, dureu::EVENT event);
    void addCanvas(const osg::Matrix& R, const osg::Matrix& T);
    void addCanvas(const osg::Matrix& R, const osg::Matrix& T, const std::string& name);

    bool loadSceneFromFile(const std::string& fname);
    bool loadSceneFromFile();
    bool writeSceneToFile(const std::string& fname) const;
    bool writeSceneToFile() const;
    void loadPhotoFromFile(const std::string& fname);

    unsigned int getMaxCanvasId() const;
    unsigned int getMaxNodeId() const;

    entity::Canvas* getCanvas(unsigned int id) const;
    entity::Canvas* getCanvas(const std::string& name) const;

    osg::Node* getNode(unsigned int id) const;
    osg::Node* getNode(const std::string& name) const;

    bool setNodeName(osg::Node* node, const std::string& name);

    bool setCanvasCurrent(entity::Canvas* cnv);
    bool setCanvasPrevious(entity::Canvas* cnv);
    entity::Canvas* getCanvasCurrent() const;
    entity::Canvas* getCanvasPrevious() const;

    unsigned int getCanvasLevel() const;
    unsigned int getPhotoLevel() const;

    void setTransformOffset(const osg::Vec3f& translate, const int mouse);
    void setTransformRotate(const osg::Vec3f& normal, const int mouse);

    void setUndoStack(QUndoStack* stack);
    QUndoStack* getUndoStack() const;

    void setFilePath(const std::string& fname);
    const std::string& getFilePath() const;
    bool isSetFilePath() const;

protected:
    bool clearUserData();
    std::string getCanvasName();
    std::string getEntityName(const std::string& name, unsigned int id) const;

    void strokeStart();
    void strokeAppend(float u, float v);
    void strokeFinish();
    bool strokeValid() const;

private:
    // todo: move userScene to a separate entity class
    // so that to save other variables such as idCanvas
    osg::ref_ptr<osg::Group> _userScene;
    osg::ref_ptr<Axes> _axes;
    osg::ref_ptr<ObserveSceneCallback> _observer;
    osg::ref_ptr<HUDCamera> _hud;
    osg::observer_ptr<entity::Canvas> _canvasCurrent;
    osg::observer_ptr<entity::Canvas> _canvasPrevious;
    unsigned int _idCanvas;
    unsigned int _idNode; // for misc entities

    QUndoStack* _undoStack;
    osg::ref_ptr<entity::Stroke> current_stroke;

    std::string m_filePath;

};

#endif // SCENE

